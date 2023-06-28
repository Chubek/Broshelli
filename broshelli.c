#include "broshelli.h"

int open_pty_pair(ptypair_t *ppair) {
  int ret;
  if ((ret = ppair->masterfd = posix_openpt(O_RDWR | O_NOCTTY)) < 0)
    return ret;
  if ((ret = unlockpt(ppair->masterfd)) < 0)
    return ret;
  if ((ret = grantpt(ppair->masterfd)) < 0)
    return ret;
  if ((ret = ptsname_r(ppair->masterfd, &ppair->slavefn[0], SLVFNAME_LEN_MAX)) <
      0)
    return ret;
  ppair->masterfstm = fdopen(ppair->masterfd, "w");
  return OPENPTY_SUCCESS;
}

void handle_relay(int signum) {
  SEND_SIGNAL_TO_PARENT(signum, int, SIGNUM_SHELL_EXEC);
}

int fork_off_slave_and_exec(char slavefn[SLVFNAME_LEN_MAX]) {
  RELAY_SIGNUM_TO_PARENT(SIGTRAP, handle_relay);
  char shellpath[SHELL_PATH_LEN], *shell = &shellpath[0];
  int slvfd, slvfdcp1, slvfdcp2, slvfdcp3, ret;
  if ((ret = slvfd = open(&slavefn[0], O_RDWR)) < 0) {
    SEND_SIGNAL_TO_PARENT(SIGNUM_SLVFD_OPEN, int, ret);
    _exit(EXIT_FAILURE);
  }
  slvfdcp1 = dup(slvfd);
  slvfdcp2 = dup(slvfd);
  slvfdcp3 = dup(slvfd);
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
    SEND_SIGNAL_TO_PARENT(SIGNUM_SHELL_EXEC, int, SHELL_DEFAULTED);
    shell = SHELL_DFL;
  }
  if ((ret = execlp(shell, shell, (char *)NULL)) < 0) {
    SEND_SIGNAL_TO_PARENT(SIGNUM_SHELL_EXEC, int, ret);
    _exit(EXIT_FAILURE);
  }
}

void wait_for_messages_from_main(pid_t cpid, FILE *masterfstm) {
  int ret;
  ssize_t iolen;
  size_t shloutlen;
  mqd_t mqdesc;
  char mqname[MQUEUE_NAME_LEN] = {0}, shellio[SHELL_IO_LEN] = {0};
  if ((ret = GET_MQ_NAME(&mqname[0], cpid)) < 0) {
    SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_NAME, int, ret);
    _exit(EXIT_FAILURE);
  }
  SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_NAME, int, MQUEUE_FNAME_SUCCESS);
  mqdesc = mq_open(mqname, O_RDWR);
  while (IS_NOT_FLAG(shellio, FLAG_TERM_MASTER)) {
    ZERO_OUT(&shellio[0], SHELL_IO_LEN);
    if ((ret = iolen = mq_receive(mqdesc, &shellio[0], SHELL_IO_LEN, NULL)) <
        0) {
      SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_RECEIVE, int, ret);
      continue;
    }
    SEND_SIGNAL_TO_PARENT(SIGNUM_MQUEUE_RECEIVE, int, MESSAGE_RECEIVE_SUCCESS);
    fwrite(&shellio[0], sizeof(char), iolen, masterfstm);
    ZERO_OUT(&shellio[0], iolen);
    while ((shloutlen =
                fread(&shellio[0], sizeof(char), SHELL_IO_LEN, masterfstm))) {
      mq_send(mqdesc, &shellio[0], shloutlen, NULL);
      ZERO_OUT(&shellio[0], SHELL_IO_LEN);
    }
    shellio[0] = (char)FLAG_TERM_MSG;
    mq_send(mqdesc, &shellio[0], sizeof(char), sizeof(char));
  }
  _exit(EXIT_SUCCESS);
}

void handle_new_terminal(int masterfdnotify) {
  pid_t cpid;
  int ret, pipefd[2] = {0};
  ptypair_t ppair;
  ZERO_OUT(&ppair, sizeof(ptypair_t));
  if ((ret = pipe(&pipefd[0])) < 0) {
    SEND_SIGNAL_TO_PARENT(SIGNUM_PIPE_OPEN, int, ret);
  }
  SEND_SIGNAL_TO_PARENT(SIGNUM_PIPE_OPEN, int, PIPE_OPEN_SUCCESS);
  if ((ret = open_pty_pair(&ppair)) < 0) {
    SEND_SIGNAL_TO_PARENT(SIGNUM_PTY_OPEN, int, ret);
    _exit(EXIT_FAILURE);
  }
  WRITE_AND_CLOSE(masterfdnotify, &ppair.masterfd, sizeof(int));
  SEND_SIGNAL_TO_PARENT(SIGNUM_PTY_OPEN, int, ret);
  if ((ret = cpid = fork()) < 0) {
    SEND_SIGNAL_TO_PARENT(SIGNUM_FORK_CHILD, int, ret);
    _exit(EXIT_FAILURE);
  }
  if (!cpid) {
    char slavefname[SLVFNAME_LEN_MAX] = {0};
    PIPE_READ(pipefd, &slavefname[0], SLVFNAME_LEN_MAX);
    fork_off_slave_and_exec(slavefname);
  } else {
    siginfo_t signalinfo;
    ZERO_OUT(&signalinfo, sizeof(siginfo_t));
    SEND_SIGNAL_TO_PARENT(SIGNUM_FORK_CHILD, int, cpid);
    PIPE_WRITE(pipefd, &ppair.slavefn[0], SLVFNAME_LEN_MAX);
    WAIT_FOR_SIGNALS(&signalinfo, MAX_SIGRT_WAITNS, SIGNUM_SLVFD_OPEN,
                     SIGNUM_SLVFD_DUP, SIGNUM_SHELL_EXEC,
                     SIGNUM_EXECVE_SUCCESS);
    UNTRACE_CHILD_PROCESS(cpid);
    SEND_SIGNAL_TO_PARENT(signalinfo.si_signo, int,
                          signalinfo.si_value.sival_int);
    WAIT_FOR_SIGNALS(&signalinfo, MAX_SIGRT_WAITNS, SIGCONT, SIGTERM);
    if (signalinfo.si_signo == SIGTERM)
      _exit(EXIT_FAILURE);
    wait_for_messages_from_main(cpid, ppair.masterfstm);
  }
}

int create_new_terminal(unsigned long retctx[RETCTX_NUM]) {
  pid_t cpid, shellpid;
  int ret, pipefd[2] = {0};
  if ((ret = pipe(&pipefd[0])) < 0) {
    return SANITIZE_PIPE_ERR_RETURN(ret);
  }
  if ((ret = cpid = fork()) < 0)
    return ret;
  if (!cpid) {
    close(pipefd[0]);
    handle_new_terminal(pipefd[1]);
  } else {
    int masterfd;
    PIPE_READ(pipefd, &masterfd, sizeof(int));
    siginfo_t signalinfo;
    WAIT_FOR_SIGNALS(&signalinfo, SIGNUM_PIPE_OPEN, SIGNUM_FORK_CHILD,
                     MAX_SIGRT_WAITNS, SIGNUM_SLVFD_OPEN, SIGNUM_SLVFD_DUP,
                     SIGNUM_SHELL_EXEC, SIGNUM_EXECVE_SUCCESS);
    if (SIG_VAL < 0) {
      return SANITIZE_SIG_ERR_RETURN(signalinfo);
    } else if (SIG_NUM == SIGNUM_FORK_CHILD && SIG_VAL >= 0) {
      shellpid = SIG_VAL;
    }
    ASSIGN_RETCTX(retctx, cpid, shellpid, masterfd, SIG_NUM, SIG_VAL);
    return RETCTX_SUCCESS;
  }
  return CREATE_DEFAULTED;
}

int kill_terminal_by_force(pid_t cpid, pid_t shellpid) {
  return kill(cpid, SIGKILL) & kill(shellpid, SIGKILL);
}

int handle_pty_io(pid_t procid, int masterfd, char *swap) {
  int ret;
  size_t iolen;
  ssize_t shliolen;
  mqd_t mqdesc;
  char mqname[MQUEUE_NAME_LEN] = {0}, *swapptr = &swap[0];
  siginfo_t signalinfo;
  if ((ret = GET_MQ_NAME(&mqname[0], procid)) < 0) {
    return MQNAME_FAIL_RET;
  }
  ZERO_OUT(swapptr, SWAP_AREA_LEN);
  while (IS_NOT_FLAG(swap, FLAG_TERM_PTY)) {
    if (IS_FLAG(swap, FLAG_NEW_CMD)) {
      iolen = 0;
      DATA_COPY(&iolen, &swapptr[1], sizeof(size_t));
      if ((ret = mq_send(mqdesc, &swap[1 + sizeof(size_t)], iolen, NULL)) < 0)
        return ret;
      ZERO_OUT(&signalinfo, sizeof(siginfo_t));
      WAIT_FOR_SIGNALS(&signalinfo, SIGNUM_MQUEUE_RECEIVE, SIGUSR1);
      if (SIG_VAL < 0) {
        return PTERM_LOOP_FAIL;
      } else if (SIG_VAL != MESSAGE_RECEIVE_SUCCESS) {
        DATA_COPY(swapptr, MSG_DELIVER_ERR_MSG_TXT, MSG_DELIVER_ERR_MSG_LEN);
      }
      *swapptr = (char)FLAG_SEND_OUT;
      shliolen = 1;
      while (shliolen >= 0 && IS_NOT_FLAG(swap, FLAG_TERM_MSG)) {
        shliolen = mq_receive(mqdesc, &swapptr[shliolen], SHELL_IO_LEN, NULL);
      }
      if (shliolen < 0) {
        return shliolen;
      }
    }
  }
  SEND_SIGNAL(procid, SIGNUM_TERMINATE_MASTER, int, masterfd);
  return TERM_FINISHED;
}