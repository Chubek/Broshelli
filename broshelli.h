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

#define STDIN_FD fileno(stdin)
#define STDOUT_FD fileno(stdout)
#define STDERR_FD fileno(stderr)

#ifndef SLVFNAME_LEN
#define SLVFNAME_LEN 64
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

#define FOREVER 1

#define SIGVAL_SHELL_DFL 7

#define SIGNUM_SHELL_EXEC SIGRTMIN
#define SIGNUM_PIPE_OPEN SIGRTMIN + 1
#define SIGNUM_PTY_OPEN SIGRTMIN + 2
#define SIGNUM_FORK_CHILD SIGRTMIN + 3
#define SIGNUM_SLVFD_DUP SIGRTMIN + 4
#define SIGNUM_SLVFD_OPEN SIGRTMIN + 5
#define SIGNUM_EXECVE_SUCCESS SIGRTMIN + 6
#define SIGNUM_MQUEUE_NAME SIGRTMIN + 7
#define SIGNUM_TERMINATE_MASTER SIGRTMIN + 8

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
#define RELAY_SIGNAL_TO_PARENT(SIGNUM, RELAY_SIGNUM) 							\
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


typedef enum {
  INIT_TERM,
  CLOSE_TERM,
  RELAY_CMD,
  RELAY_OUTPUT,
  RELAY_PTYPID,
} relaydir_t;

typedef struct {
  int masterfd;
  int slavefd;
  char slavefname[SLVFNAME_LEN];
  pid_t slavepid;
  FILE *masterfstm;
} ptypair_t;

typedef struct {
  pid_t ptypid;
  uint32_t msglen;
  relaydir_t directive;
  char io[SHELL_IO_LEN];
} iorelay_t;

#endif