#ifndef SIGDEF_H
#define SIGDEF_H

#ifndef MAX_SIGRT_WAITNS
#define MAX_SIGRT_WAITNS 5000
#endif

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


#endif