#ifndef BROSHELLI_NIXPTY_H
#define BROSHELLI_NIXPTY_H

#define _XOPEN_SOURCE 600
#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifndef SLVFNAME_LEN_MAX
#define SLVFNAME_LEN_MAX 64
#endif

#ifndef SHELL_PATH_LEN
#define SHELL_PATH_LEN 256
#endif

#ifndef MQUEUE_NAME_LEN
#define MQUEUE_NAME_LEN 512
#endif

#ifndef SHELL_IO_LEN
#define SHELL_IO_LEN 256000
#endif

#ifndef SHELL_ENV
#define SHELL_ENV "SHELL"
#endif

#ifndef MAX_SIGRT_WAITNS
#define MAX_SIGRT_WAITNS 5000
#endif

#ifndef SHELL_DFL
#define SHELL_DFL "/usr/bin/bash"
#endif

#ifndef MQ_FILE_FMT
#define MQ_FILE_FMT "%d.btymq"
#endif

#ifndef ZERO_OUT
#define ZERO_OUT(BUFF, LEN) memset(BUFF, 0, LEN)
#endif

#ifndef DATA_COPY
#define DATA_COPY(DST_BUFF, SRC_BUFF, LEN) memmove(DST_BUFF, SRC_BUFF, LEN)
#endif

#if !defined(MSG_DELIVER_ERR_MSG_TXT) && !defined(MSG_DELIVER_ERR_MSG_LEN)
#define MSG_DELIVER_ERR_MSG_TXT "Failed to relay command to shell process"
#define MSG_DELIVER_ERR_MSG_LEN 45
#endif

#ifndef SWAP_AREA_LEN
#define SWAP_AREA_LEN 64000
#endif

#ifndef __SIZEOF_LONG_LONG__
#define __SIZEOF_LONG_LONG__ sizeof(unsigned long long)
#endif

#define STDIN_FD fileno(stdin)
#define STDOUT_FD fileno(stdout)
#define STDERR_FD fileno(stderr)

#define FLAG_NEW_CMD 255
#define FLAG_SEND_OUT 128
#define FLAG_TERM_MSG 135
#define FLAG_TERM_PTY 221
#define FLAG_TERM_MASTER 255

#define OPENPTY_SUCCESS 1
#define MESSAGE_RECEIVE_SUCCESS 1
#define TERM_CREATE_SUCCESS 1
#define MQUEUE_FNAME_SUCCESS 1
#define CREATE_DEFAULTED 2
#define SHELL_DEFAULTED 123
#define PIPE_OPEN_SUCCESS 132

#define MQNAME_FAIL_RET -1
#define PTERM_LOOP_FAIL -1
#define TERM_FINISHED 1
#define RETCTX_PIPE_ERR 2
#define RETCTX_SUCCESS 7
#define RETCTX_ERR 6
#define RETCTX_NUM 5

#define SIGNUM_SHELL_EXEC SIGRTMIN
#define SIGNUM_PIPE_OPEN SIGRTMIN + 1
#define SIGNUM_PTY_OPEN SIGRTMIN + 2
#define SIGNUM_FORK_CHILD SIGRTMIN + 3
#define SIGNUM_SLVFD_DUP SIGRTMIN + 4
#define SIGNUM_SLVFD_OPEN SIGRTMIN + 5
#define SIGNUM_EXECVE_SUCCESS SIGRTMIN + 6
#define SIGNUM_MQUEUE_NAME SIGRTMIN + 7
#define SIGNUM_TERMINATE_MASTER SIGRTMIN + 8
#define SIGNUM_MQUEUE_RECEIVE SIGRTMIN + 9

#define SIG_VAL signalinfo.si_value.sival_int
#define SIG_NUM signalinfo.si_signo

#define SANITIZE_PIPE_ERR_RETURN(PIPE_ERR)                                     \
  ((-1 * PIPE_ERR) << 2) | RETCTX_PIPE_ERR
#define SANITIZE_SIG_ERR_RETURN(SIGVAL)                                        \
  ((SIGVAL.si_signo << 6) | ((-1 * SIGVAL.si_value.sival_int)) << 3) |         \
      RETCTX_ERR

#define IS_FLAG(BUFF, FLAG) ((unsigned char)BUFF[0] == FLAG)
#define IS_NOT_FLAG(BUFF, FLAG) ((unsigned char)BUFF[0] != FLAG)

#define GET_MQ_NAME(BUF, PID) snprintf(BUF, MQUEUE_NAME_LEN, MQ_FILE_FMT, PID)

#define WAIT_FOR_SIGNALS(INFO, NSECS, ...)                                     \
  do {                                                                         \
    int signal, signals[] = {__VA_ARGS__, 0}, *siptr = &signals[0];            \
    sigset_t signalset;                                                        \
    struct timespec time = (struct timespec){.tv_sec = 0, .tv_nsec = NSECS};   \
    sigemptyset(&signalset);                                                   \
    while ((signal = *siptr++))                                                \
      sigaddset(&signalset, signal);                                           \
    sigtimedwait(&signalset, INFO, &time);                                     \
  } while (0)

#define SEND_SIGNAL(PROCNUM, SIGNUM, SVALTYY, SIGVAL)                          \
  sigqueue(PROCNUM, SIGNUM, (union sigval){.sival_##SVALTYY = SIGVAL})
#define SEND_SIGNAL_TO_PARENT(...) SEND_SIGNAL(getppid(), __VA_ARGS__)
#define RAISE_SIGNAL_TO_SELF(...) SEND_SIGNAL(getpid(), __VA_ARGS__)
#define RELAY_SIGNUM_TO_PARENT(SIGNUM, SIGNAL_HANDLER)                         \
  do {                                                                         \
    const struct sigaction signalaction =                                      \
        (struct sigaction){.sa_handler = &SIGNAL_HANDLER};                     \
    sigaction(SIGNUM, &signalaction, NULL);                                    \
  } while (0)
#define SET_RT_SIGNAL_ACTION(SIGNUM, SA_HANDLER_NAME)                          \
  do {                                                                         \
    struct sigaction signalaction = (struct sigaction){                        \
        .sa_handler = &SA_HANDLER_NAME, .sa_flags = SA_SIGINFO};               \
    sigaction(SIGNUM, &signalaction, NULL);                                    \
  } while (0)

#define TRACE_CURR_PROCESS() ptrace(PTRACE_TRACEME, NULL, NULL, NULL)
#define UNTRACE_CHILD_PROCESS(PID) ptrace(PTRACE_DETACH, PID, NULL, NULL)

#define PIPE_READ(PIPE, ...)                                                   \
  do {                                                                         \
    close(PIPE[1]);                                                            \
    read(PIPE[0], __VA_ARGS__);                                                \
    close(PIPE[0]);                                                            \
  } while (0)
#define PIPE_WRITE(PIPE, ...)                                                  \
  do {                                                                         \
    close(PIPE[0]);                                                            \
    write(PIPE[1], __VA_ARGS__);                                               \
    close(PIPE[1]);                                                            \
  } while (0)

#define READ_AND_CLOSE(FD, ...)                                                \
  do {                                                                         \
    read(FD, __VA_ARGS__);                                                     \
    close(FD);                                                                 \
  } while (0)
#define WRITE_AND_CLOSE(FD, ...)                                               \
  do {                                                                         \
    write(FD, __VA_ARGS__);                                                    \
    close(FD);                                                                 \
  } while (0)

#define ASSIGN_RETCTX(RETCTX, ...)                                             \
  do {                                                                         \
    unsigned long ctx, ctxs[RETCTX_NUM + 1] = {__VA_ARGS__, 0},                \
                                         *ctxptr = &ctxs[0],                   \
                                         *ctxhead = ctxptr;                    \
    while ((ctx = *ctxptr++))                                                  \
      RETCTX[ctxptr - ctxhead] = ctx;                                          \
  } while (0)

typedef struct PtyPair {
  int masterfd;
  int slavefd;
  char slavefn[SLVFNAME_LEN_MAX];
  pid_t slavepid;
  FILE *masterfstm;
} ptypair_t;

#endif