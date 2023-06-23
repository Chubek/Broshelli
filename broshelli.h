#ifndef BROSHELLI_H
#define BROSHELLI_H

#define _XOPEN_SOURCE 600
#define _GNU_SOURCE
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>


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
	FILE *smaster;
	union {
		uintptr_t arena[ARENA_SIZE];
		struct {
			uint16_t ptyid;
			uint32_t size;
			char *command;
		} msg;
	} browserio;
} btyctx_t;













#endif