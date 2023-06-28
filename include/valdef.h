#ifndef VALDEF_H
#define VALDEF_H

#ifndef __SIZEOF_LONG_LONG__
#define __SIZEOF_LONG_LONG__ sizeof(unsigned long long)
#endif

#ifndef SHELL_ENV
#define SHELL_ENV "SHELL"
#endif

#ifndef SHELL_DFL
#define SHELL_DFL "/usr/bin/bash"
#endif


#define FLAG_NEW_CMD 255
#define FLAG_SEND_OUT 128
#define FLAG_TERM_MSG 135
#define FLAG_TERM_PTY 221
#define FLAG_TERM_MASTER 255

#define OPENPTY_SUCCESS 1
#define MESSAGE_RECEIVE_SUCCESS 1
#define TERM_CREATE_SUCCESS 1
#define MQUEUE_FNAME_SUCCESS 1
#define CREATE_DEFAULTED 2
#define SHELL_DEFAULTED 123
#define PIPE_OPEN_SUCCESS 132

#define MQNAME_FAIL_RET -1
#define PTERM_LOOP_FAIL -1
#define TERM_FINISHED 1
#define RETCTX_PIPE_ERR 2
#define RETCTX_SUCCESS 7
#define RETCTX_ERR 6
#define RETCTX_NUM 5

#define IS_FLAG(BUFF, FLAG) ((unsigned char)BUFF[0] == FLAG)
#define IS_NOT_FLAG(BUFF, FLAG) ((unsigned char)BUFF[0] != FLAG)


#define ASSIGN_RETCTX(RETCTX, ...)                                             \
  do {                                                                         \
    unsigned long ctx, ctxs[RETCTX_NUM + 1] = {__VA_ARGS__, 0},                \
                                         *ctxptr = &ctxs[0],                   \
                                         *ctxhead = ctxptr;                    \
    while ((ctx = *ctxptr++))                                                  \
      RETCTX[ctxptr - ctxhead] = ctx;                                          \
  } while (0)



#endif