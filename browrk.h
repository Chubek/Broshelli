#ifndef BROWRK_H
#define BROWRK_H

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

#ifndef TMP_PROC_DIR_FMT
#define TMP_PROC_DIR_FMT "/tmp/bty/%lu.btyproc"
#endif

#ifndef TMP_PROC_DIR_LEN
#define TMP_PROC_DIR_LEN 48
#endif


typedef struct {
	pid_t pid;
	char slavename[SPTYNAEM_LEN];
	char shell[SHESTR_LEN];
	char args[SHELARGS_LEN];
	int slavefd;
	FILE *slaves;
} bproc_t;

typedef enum {
	ACT_STORE = 0,
	ACT_RETRIEVE = 1,
	ACT_UNLINK = 3,
} bsttact_t;


void termination_handler(int signum);
int fork_off_and_exec_shell(bproc_t *proc);
int kill_exec_process(bproc_t *proc);

#endif