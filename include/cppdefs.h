#ifndef CPPDEFS_H
#define CPPDEFS_H

#ifndef SHELL_ENV
#define SHELL_ENV "SHELL"
#endif

#ifndef SHELL_DFL
#define SHELL_DFL "/usr/bin/bash"
#endif

#ifndef PTY_NAME_LEN_MAX
#define PTY_NAME_LEN_MAX 64
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

#ifndef DEVICE_DIR
#define DEVICE_DIR "/dev"
#endif

#ifndef BSDPTY_MASTER_PATT
#define BSDPTY_MASTER_PATT "pty"
#endif

#ifndef BSDPTY_SLAVE_PATT
#define BSDPTY_SLAVE_PATT "tty"
#endif

#ifndef LEN_BSDPTY_PATT
#define LEN_BSDPTY_PATT 3
#endif

#ifndef MAX_SIGRT_WAITNS
#define MAX_SIGRT_WAITNS 5000
#endif


#ifdef CONPTY
#if !defined(__windows__) && !defined(FORCE_HOST)
#error "CONPTY is only available on Windows hosts, compile with FORCE_HOST to ignore this error"
#elif !defined(__windows__) && defined(FORCE_HOST)
#define __windows__
#endif
#elif NIXPTY
#if !defined(__unix__) && !defined(FORCE_HOST)
#error "NIXPTY is only available on POSIX hosts, compile with FORCE_HOST to ignore this error"
#elif !defined(__unix__) && defined(FORCE_HOST)
#define __unix__
#endif
#elif defined(BROPTY) && !defined(__wasm__)
#define __wasm__
#endif



#endif