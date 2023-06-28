#ifndef DYNDEF_H
#define DYNDEF_H


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


#endif