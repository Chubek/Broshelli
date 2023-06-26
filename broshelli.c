#include "broshelli.h"

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
	RELAY_SIGNUM_TO_PARENT(SIGTRAP, SIGNUM_EXECVE_SUCCESS);
	char shellpath[SHELL_PATH_LEN], *shell = &shellpath[0];
	int slvfd, slvfdcp1, slvfdcp2, slvfdcp3, ret;
	if ((ret = slvfd = open(slavefname, O_RDWR)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_SLVFD_OPEN, int, ret);
		_exit(EXIT_FAILURE);
	}
	slvfdcp1 = dup(slavefd);
	slvfdcp2 = dup(slavefd);
	slvfdcp3 = dup(slavefd);
	if ((ret = dup2(STDIN_FD, slvfdcp1)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_SLVFD_DUP, int, ret);
		_exit(EXIT_FAILURE);
	}
	if ((ret = dup2(STDOUT_FD, slvfdcp2)) < 0)
		SEND_SIGNAL_TO_PARENT(SIGNUM_SLVFD_DUP, int, ret);
		_exit(EXIT_FAILURE);
	if ((ret = dup2(STDERR_FD, slvfdcp3)) < 0)
		SEND_SIGNAL_TO_PARENT(SIGNUM_SLVFD_DUP, int, ret);
		_exit(EXIT_FAILURE);
	if (!(shell = getenv(SHELL_ENV))) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_SHELL_EXEC, int, SIGNUM_SHELL_DFL);
		shell = SHELL_DFL;
	}
	if ((ret = execlp(shell, shell, (char*)NULL)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_SHELL_EXEC, int, ret);
		_exit(EXIT_FAILURE);
	}
}

void wait_for_messages_from_main(pid_t cpid, FILE *masterfstm) {
	int ret;
	ssize_t iolen;
	size_t shloutlen;
	mqd_t mqdesc;
	char mqname[MQUEUE_NAME_LEN], shellio[SHELL_IO_LEN], termflag[__SIZEOF_LONG_LONG__] = TERM_SHELL_OUT_FLAG;
	memset(&mqname[0], 0, MQUEUE_NAME_LEN);
	if ((ret = snprintf(&mqname[0], MQUEUE_NAME_LENGHT, "%d.btymq", mypid)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_NAME, int, ret);
		_exit(EXIT_FAILURE);
	}
	SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_NAME, int, ret);
	mqdesc = mq_open(mqname, O_RDWR);
	while (FOREVER) {
		memset(&shellio[0], 0, SHELL_IO_LEN);
		if ((ret = iolen = mq_receive(mqdesc, &shellio[0], SHELL_IO_LEN, NULL)) < 0) {
			SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_RECEIVE, int, ret);
			continue;
		}
		SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_RECEIVE, int, MESSAGE_RECEIVE_SUCCESS);
		fwrite(&shellio[0], sizeof(char), iolen, masterfstm);
		memset(&shellio[0], 0, iolen);
		while ((shloutlen = fread(&shellio[0], sizeof(char), SHELL_IO_LEN, masterfstm))) {
			mq_send(mqdesc, &shellio[0], shloutlen, NULL);
		}
		mq_send(mqdesc, TERM_SHELL_OUT_FLAG, __SIZEOF_LONG_LONG__);
	}
}

void sigrt_handle_termination(int signum, siginfo_t *signalinfo, void *unused) {
	if (signum == SIGNUM_TERMINATE_MASTER) {
		close(signalinfo->si_value.sival_int);
		_exit(EXIT_SUCCESS);
	}
}

void handle_new_terminal(int masterfdnotify) {
	pid_t cpid;
	int ret, pipefd[2];
	ptypair_t ppair;
	memset(&ppair, 0, sizeof(ptypair_t));
	if ((ret = open_pty_pair(&ppair)) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_PTY_OPEN, int, ret);
		_exit(EXIT_FAILURE);
	}
	write(masterfdnotify, &ppair->masterfd, sizeof(int));
	close(masterfdnotify);
	SEND_SIGNAL_TO_PARENT(SIGNUM_PTY_OPEN, int, ret);
	if ((ret = cpid = fork()) < 0) {
		SEND_SIGNAL_TO_PARENT(SIGNUM_FORK_CHILD, int, ret);
		_exit(EXIT_FAILURE);
	}
	if (!cpid) {
		char slavefname[SLVFNAME_LEN];
		memset(&slavefname[0], 0, SLVGNAME_LEN);
		close(pipefd[1]);
		read(pipefd[0], &slavefname[0], SLVFNAME_LEN);
		fork_off_slave_and_exec(&slavefname[0]);
	} else {
		SET_RT_SIGNAL_ACTION(SIGNUM_TERMINATE_MASTER, sigrt_handle_termination);
		siginfo_t signalinfo;
		memset(&signalinfo, 0, sizeof(siginfo_t));
		SEND_SIGNAL_TO_PARENT(SIGNUM_FORK_CHILD, int, cpid);
		close(pipefd[0]);
		write(pipefd[1], &ppair.slavefname[0], SLVFNAME_LEN);
		WAIT_FOR_SIGNALS(&signalinfo, MAX_SIGRT_WAITNS, SIGNUM_SLVFD_OPEN, SIGNUM_SLVFD_DUP, SIGNUM_SHELL_EXEC, SIGNUM_EXECVE_SUCCESS);
		UNTRACE_CHILD_PROCESS(cpid);
		SEND_SIGNAL_TO_PARENT(signalinfo.si_signo, int, signalinfo.si_value.sival_int);
		WAIT_FOR_SIGNALS(&signalinfo, MAX_SIGRT_WAITNS, SIGCONT, SIGTERM);
		if (signalinfo.si_signo == SIGTERM)
			_exit(EXIT_FAILURE);
		wait_for_messages_from_main(cpid, ppair.masterfstm);
	}
}


unsigned long create_new_terminal() {
	pid_t cpid;
	int ret, pipefd[2];
	if ((ret = cpid = form()) < 0)
		return ret;
	if (!cpid) {
		close(pipefd[0]);
		handle_new_terminal(pipefd[1]);
	} else {
		int masterfd;
		close(pipefd[1]);
		read(pipefd[0], &masterfd, sizeof(int));
		struct siginfo signalinfo;
		WAIT_FOR_SIGNALS(&signalinfo, SIGNUM_FORK_CHILD, MAX_SIGRT_WAITNS, SIGNUM_SLVFD_OPEN, SIGNUM_SLVFD_DUP, SIGNUM_SHELL_EXEC, SIGNUM_EXECVE_SUCCESS);
		if (signalinfo.si_value.int_val < 0) {
			return SANITIZE_SIG_RETURN(signalinfo);
		}
		return CONCAT_RET_FDESCS(cpid, masterfd);
	}
}

int handle_io_from_browser(pid_t procid, int masterfd, char *swap) {
	int ret;
	size_t iolen;
	mqd_t mqdesc;
	char mqname[MQUEUE_NAME_LEN], *swapptr = &swap[0];
	struct siginfo signalinfo;
	memset(&mqname[0], 0, MQUEUE_NAME_LEN);
	if ((ret = snprintf(&mqname[0], MQUEUE_NAME_LENGHT, "%d.btymq", procid)) < 0) {
		return MQNAME_FAIL_RET;
	}
	while (FOREVER) {
		if ((unsigned char)*swapptr == FLAG_NEW_MSG) {
			iolen = 0;
			memmove(&iolen, &swapptr[1], sizeof(size_t));
			if ((ret = mq_send(mqdesc, &shellio[1 + sizeof(size_t)], iolen, NULL)) < 0)
				return ret;
			memset(&signalinfo, 0, sizeof(struct siginfo));
			WAIT_FOR_SIGNALS(&signalinfo, SIGNUM_MQUEUE_RECEIVE);
			if (signalinfo.si_value.sival_int < 0) {
				return PTERM_LOOP_FAIL;
			} else if (signalinfo.si_value.sival_int != MESSAGE_RECEIVE_SUCCESS) {
				memmove(swapptr, MSG_DELIVER_ERR_MSG_TXT, MSG_DELIVER_ERR_MSG_LEN);
			}
			*swapptr = (char)FLAG_SEND_MSG;
			if ((ret = mq_receive(mqdesc, &swapptr[1], SWAP_AREA_LEN, NULL)))
				return ret;
		} else if ((unsigned char)*swapptr == FLAG_STOP_CLOSE) {
			SEND_SIGNAL_TO_CHILD(procid, SIGNUM_TERMINATE_MASTER, int, masterfd);
			return TERM_FINISHED;
		}
	}
}