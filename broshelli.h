#ifndef BROSHELLI_H
#define BROSHELLI_H

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
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ptrace.h>
 #include <sys/stat.h>

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

#if  !defined(MSG_DELIVER_ERR_MSG_TXT) && !defined(MSG_DELIVER_ERR_MSG_LEN)
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
#define FLAG_TERM_PTY 221
#define FLAG_TERM_MASTER 255

#define MESSAGE_RECEIVE_SUCCESS 1
#define TERM_CREATE_SUCCESS 1
#define MQUEUE_FNAME_SUCCESS 1
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
#define SIGNUM_PIPE_OPEN SIGRTMIN + 10

#define SIG_VAL signalinfo.si_value.int_val
#define SIG_NUM signalinfo.si_no

#define SANITIZE_PIPE_ERR_RETURN(PIPE_ERR) ((-1 * PIPE_ERR) << 2) | RETCTX_PIPE_ERR
#define SANITIZE_SIG_ERR_RETURN(SIGVAL) ((SIGVAL.si_signo  << 6) | ((-1 * SIGVAL.si_value.si_int)) << 3) | RETCTX_ERR

#define IS_FLAG(BUFF, FLAG) (unsigned char)BUFF[0] == FLAG
#define IS_NOT_FLAG(BUFF, FLAG) (unsigned char)BUFF[0] != FLAG


#define GET_MQ_NAME(BUF, PID) snprintf(BUF, MQUEUE_NAME_LEN, MQ_FILE_FMT, PID)

#define WAIT_FOR_SIGNALS(INFO, NSECS, ...)                                             \
  do {                                                                         \
    char signal, signals[] = {__VA_ARGS__, 0};                                 \
    struct sigset_t signalset;                                                 \
    struct tspec =  (struct timespec){.tv_sec = 0, .tv_nsec = NSECS}		   \
    sigfillset(&signalset);                                                    \
    while ((signal = *signals++))                                              \
      sigaddset(&signalset, (int)signal);                                      \
    sigtimedwait(&signalset, INFO, &tspec);                                    \
  } while (0)

#define SEND_SIGNAL(PROCNUM, SIGNUM, SVALTYY, SIGVAL)                          \
  sigqueue(PROCNUM, SIGNUM, (union sigval){.sival_##SVALTYY = SIGVAL})
#define SEND_SIGNAL_TO_PARENT(...) SEND_SIGNAL(getppid(), __VA_ARGS__)
#define RAISE_SIGNAL_TO_SELF(...) SEND_SIGNAL(getpid(), __VA_ARGS__)
#define RELAY_SIGNUM_TO_PARENT(SIGNUM, RELAY_SIGNUM) 							\
  do {																			\
  	void signal_handler(int signum) {											\
  		SEND_SIGNAL_TO_PARENT(signum, int, RELAY_SIGNUM);						\
  	}																			\
	struct sigaction signalaction =  (struct sigaction){.sa_handler = &signal_handler};	\
	sigaction(SIGNUM, &signalaction, NULL);												 \
  } while (0)
#define SET_RT_SIGNAL_ACTION(SIGNUM, SA_HANDLER_NAME)							\
  do {																					\
  	struct sigaction signalaction = (struct sigaction){.sa_handler = &SA_HANDLER_NAME, .sa_flags = SA_SIGINFO};	\
  	sigaction(SIGNUM, &signalaction, NULL);												\
  } while (0)

#define TRACE_CURR_PROCESS() ptrace(PTRACE_TRACEME, NULL, NULL, NULL)
#define UNTRACE_CHILD_PROCESS(PID) ptrace(PTRAC_DETACH, PID, NULL, NULL)


#define ASSIGN_RETCTX(RETCTX, ...) \
  &RETCTX[0] = {__VA_ARGS__}

typedef struct {
	char filename[SLVFNAME_LEN_MAX];
	size_t fnlen;
} slvfn_t;

typedef struct {
  int masterfd;
  int slavefd;
  slvfn_t slavefn;
  pid_t slavepid;
  FILE *masterfstm;
} ptypair_t;


#endif