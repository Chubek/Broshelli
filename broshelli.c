#include "broshelli.h"
#include "fjwfmt.h"

void parse_browser_message(iorelay_t *relay) {
	memset(relay, 0, sizeof(*relay));
	fread(&relay->msglen, sizeof(uint32_t), 1, stdin);
	jscanf("{ptypid':%i,'io':%s}", &relay->ptypid, &relay->io[0]);
}

void marshal_browser_message(iorelay_t *relay) {
	fwrite(&relay->relay, sizeof(uint32_t), 1, stdout);
	jprintf("{'ptypid':%i,'io':%s}", relay->ptypid, &relay->io[0]);	
}

int open_pty_pair(ptypair_r ppair) {
	int ret;
	if ((ret = ppair->masterfd = posix_openpt(O_RDWR | O_NOCTTY)) < 0)
		return ret;
	if ((ret = unlockpt(ppair->masterfd)) < 0)
		return ret;
	if ((ret = grantpt(ppair->masterfd)) < 0)
		return ret;
	if ((ret = ptsname_r(ppair->masterfd, &ppair->slavefname[0], SLVFN_LEN)) < 0)
		return ret;
	if ((ret = ppair->masterfstm = fopen(ppair->masterfd, "w")) < 0)
		return ret;
}

int fork_off_slave_and_exec(char *slavefname) {
	char shellpath[SHELL_PATH_LEN], *shell = &shellpath[0];
	int slvfd, slvfdcp1, slvfdcp2, slvfdcp3, ret;
	if ((ret = slvfd = open(slavefname, O_RDWR)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNAL_SLVFD_OPEN, int, ret);
		_exit(EXIT_FAILURE);
	}
	slvfdcp1 = dup(slavefd);
	slvfdcp2 = dup(slavefd);
	slvfdcp3 = dup(slavefd);
	if ((ret = dup2(STDIN_FD, slvfdcp1)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNAL_SLVFD_DUP, int, ret);
		_exit(EXIT_FAILURE);
	}
	if ((ret = dup2(STDOUT_FD, slvfdcp2)) < 0)
		SEND_SIGNAL_TO_PARENT(SIGNAL_SLVFD_DUP, int, ret);
		_exit(EXIT_FAILURE);
	if ((ret = dup2(STDERR_FD, slvfdcp3)) < 0)
		SEND_SIGNAL_TO_PARENT(SIGNAL_SLVFD_DUP, int, ret);
		_exit(EXIT_FAILURE);
	if (!(shell = getenv(SHELL_ENV))) {
		SEND_SIGNAL_TO_PARENT(SIGNAL_SHELL_EXEC, int, SIGNAL_SHELL_DFL);
		shell = SHELL_DFL;
	}
	if ((ret = execlp(shell, shell, (char*)NULL)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNAL_SHELL_EXEC, int, ret);
		_exit(EXIT_FAILURE);
	}
}

int handle_new_terminal() {
	pid_t cpid;
	int ret, pipefd[2];
	ptypair_t ppair;
	memset(&ppair, 0, sizeof(ptypair_t));
	if ((ret = open_pty_pair(&ppair)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_PTY_OPEN, int, ret);
		_exit(EXIT_FAILURE);
	}
	SEND_SIGNAL_TO_PARENT(SIGNUM_PTY_OPEN, int, ret);
	if ((ret = cpid = fork()) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNAL_FORK_CHILD, int, ret);
		_exit(EXIT_FAILURE);
	}
	if (!cpid) {
		char slavefname[SLVFNAME_LEN];
		memset(&slavefname[0], 0, SLVGNAME_LEN);
		close(pipefd[1]);
		read(&slavefname[0], pipefd[0], SLVFNAME_LEN);
		fork_off_slave_and_exec(&slavefname[0]);
	} else {
		SEND_SIGNAL_TO_PARENT(SIGNAL_FORK_CHILD, int, cpid);
		siginfo_t signalinfo;
		memset(&signalinfo, 0, sizeof(siginfo_t));
		WAIT_FOR_SIGNALS(&signalinfo, SIGNAL_SLVFD_OPEN, SIGNAL_SLVFD_DUP, SIGNAL_SHELL_EXEC);
		
	}
}


int main() {
	btyppair_t ppair;
	parse_browser_message(&ppair);
	marshal_browser_message(&ppair);
}