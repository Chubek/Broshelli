#ifndef BROSHELLI_H
#define BROSHELLI_H

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDINT_H
#define STDINT_H
#include <stdint.h>
#endif

#ifndef SIGNAL_H
#define SIGNAL_H
#include <signal.h>
#endif

#ifndef WCHAR_H
#define WCHAR_H
#include <wchar.h>
#endif


#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef TERMIOS_H
#define TERMIOS_H
#include <termios.h>
#endif

#ifndef UNISTD_H
#define UNISTD_H
#include <unistd.h>
#endif

#ifndef TYPES_H
#define TYPES_H
#include <sys/types.h>
#endif

#if !defined(STDIN_FD) && !defined(STDOUT_FD) && !defined(STDERR_FD)
#define STDIN_FD 0
#define STDOUT_FD 1
#define STDERR_FD 2
#endif

#ifndef TMPSTATEFPATH_LEN
#define TMPSTATEFPATH_LEN 64
#endif

#ifndef EXIT_ERR
#define EXIT_ERR 1
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef SLVFN_LEN
#define SLVFN_LEN 64
#endif

#ifndef ARENA_SIZE
#define ARENA_SIZE 64000
#endif

#ifndef SHELL_ENV
#define SHELL_ENV "SHELL"
#endif

#ifndef SHELL_DFL
#define SHELL_DFL "/usr/bin/bash"
#endif

#ifdef EXECV_PATH
#define EXECV_FN execvp
#else
#define EXECV_FN execv
#endif

#ifndef EXEC_ARGS
#define EXEC_ARGS NULL
#endif

typedef enum {
	STORE_CTX,
	RETRIEVE_CTX,
	REMOVE_CTX,
	READ_BROWSERIO,
	WRITE_BROWSERIO,
} btydir_t;

typedef struct {
	int masterfd;
	int slavefd;
	char slavefname[SLVFN_LEN];
	pid_t slavepid;
	FILE *tmpf;
	union {
		uintptr_t arena[ARENA_SIZE];
		struct {
			uint16_t ptyid;
			uint32_t size;
			char *command;
		} browsermsg;
	} browserio;
} btyctx_t;













#endif