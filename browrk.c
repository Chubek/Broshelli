#include "broproc.h"

int bproc_state_io(bproc_t *proc, bsttact_t action) {
	pid_t spid = getpid();
	FILE *fstrm;
	char fpath[TMP_PROC_DIR_LEN] = {0};
	sprintf(&fpath[0], TMP_PROC_DIR_FMT, (uint64_t)spid);
	switch (action) {
		case ACT_STORE:
			fwrite(proc, sizeof(*proc), 1, fstrm);
		case ACT_RETRIEVE:
			fread(proc, sizeof(*proc), 1, fstrm);
		case ACT_UNLINK:
			unlink();
	}
}

void termination_handler(int signum) {
	bproc_t proc;
	bproc_state_io(&proc, ACT_RETRIEVE);
	close(proc.slavefd);
	fclose(proc.slaves);
	bproc_state_io(NULL, ACT_UNLINK);
	_exit();
}

int fork_off_and_exec_shell(bproc_t *proc) {
	if ((proc->pid = fork()) < 0)
		return proc->pid;
	if (!proc->pid) {
		signal(SIGINT, termination_handler);
		proc->slavefd = open(&proc->slavename[0], O_RDWR);
		proc->slaves = fdopen(proc->slavefd, "w");
		bproc_state_io(proc, ACT_STORE);
		execv(&proc->shell[0], proc->args[0]);
	}
}

