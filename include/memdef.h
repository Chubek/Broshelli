#ifndef MEMDEF_H
#define MEMDEF_H

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


#endif