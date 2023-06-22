#ifndef BROPROC_H
#define BROPROC_H

#ifndef UNISTD_H
#define UNISTD_H
#include <unistd.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef SIGNAL_H
#define SIGNAL_H
#include <signal.h>
#endif


#ifndef TYPES_H
#define TYPES_H
#include <sys/types.h>
#endif

#ifndef SHELLSTR_LEN
#define SHELLSTR_LEN 16
#endif

#ifndef SHELLARGS_LEN
#define SHELLARGS_LEN 32
#endif

#ifndef SPTYNAME_LEN
#define SPTYNAME_LEN 64
#endif

#ifndef ERR
#define ERR -1
#endif

typedef struct {
	pid_t pid;
	char slavename[SPTYNAEM_LEN];
	char shell[SHESTR_LEN];
	char args[SHELARGS_LEN];
	int slavefd;
	FILE *slaves;
} bproc_t;

static inline int fork_off_and_exec_shell(bproc_t *proc) {
	if ((proc->pid = fork()) < 0)
		return proc->pid;
	if (!proc->pid) {
		static inline void kill_exec_handler(int sig) {
			close(proc->slavefd);
			_exit();
		}
		signal(SIGINT, kill_exec_handler);
		proc->slavefd = open(&proc->slavename[0], O_RDWR);
		execv(proc->shell, proc->args);
	}
}

static inline int kill_exec_process(bproc_t *proc) {
	return kill(proc->pid, SIGINT);
}

#endif