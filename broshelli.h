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
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>
#include <time.h>


#define STDIN_FD fileno(stdin)
#define STDOUT_FD fileno(stdout)
#define STDERR_FD fileno(stderr)

#ifndef SLVFNAME_LEN
#define SLVFNAME_LEN 64
#endif

#ifndef SHELL_PATH_LEN
#define SHELL_PATH_LEN 256
#endif

#ifndef ARENA_SIZE
#define ARENA_SIZE 256000
#endif

#ifndef SHELL_ENV
#define SHELL_ENV "SHELL"
#endif

#ifndef SHELL_DFL
#define SHELL_DFL "/usr/bin/bash"
#endif

#define SIGVAL_SHELL_DFL 7

#define SIGNUM_SHELL_EXEC SIGRTMIN
#define SIGNUM_PIPE_OPEN SIGRTMIN + 1
#define SIGNUM_PTY_OPEN SIGRTMIN + 2
#define SIGNUM_FORK_CHILD SIGRTMIN + 3
#define SIGNUM_SLVFD_DUP SIGRTMIN + 4
#define SIGNUM_SLVFD_OPEN SIGRTMIN + 5

#define WAIT_FOR_SIGNALS(INFO, SECS, NSECS, ...)                                             \
  do {                                                                         \
    char signal, signals[] = {__VA_ARGS__, 0};                                 \
    struct sigset_t signalset;                                                 \
    struct tspec =  (struct timespec){.tv_sec = SECS, .tv_nsec = NSECS}		   \
    sigfillset(&signalset);                                                    \
    while ((signal = *signals++))                                              \
      sigaddset(&signalset, (int)signal);                                      \
    sigtimedwait(&signalset, INFO, &tspec);                                    \
  } while (0)

#define SEND_SIGNAL(PROCNUM, SIGNUM, SVALTYY, SIGVAL)                          \
  sigqueue(PROCNUM, SIGNUM, (union sigval){.sival_##SVALTYY = SIGVAL})
#define SEND_SIGNAL_TO_PARENT(...) SEND_SIGNAL(getppid(), __VA_ARGS__)
#define RAISE_SIGNAL_TO_SELF(...) SEND_SIGNAL(getpid(), __VA_ARGS__)

#define NEW_SIGINFO(HANDLE_PTR)

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
  char io[ARENA_SIZE];
} iorelay_t;

#endif