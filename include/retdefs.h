#ifndef RETDEFS_H
#define RETDEFS_H

#define FDSCDUP_SUCCESS_STDIO 15
#define FDSCDUP_FAIL_STDIN 1
#define FDSCDUP_FAIL_STDOUT 3
#define FDSCDUP_FAIL_STDERR 7
#define FDSCDUP_MASK_SHFL 4

#define POSIXMASTERPTY_SUCCESS_OPEN 15
#define POSIXMASTERPTY_FAILURE_OPEN 1
#define POSIXMASTERPTY_FAILURE_UNLOCK 3
#define POSIXMASTERPTY_FAILURE_GRANT 7
#define POSIXMASTERPTY_MASK_SHFL 4

#define POSIXSLAVEPTY_SUCCESS_OPEN 7
#define POSIXSLAVEPTY_FAILURE_PTSNAME 1
#define POSIXSLAVEPTY_FAILURE_OPEN 3
#define POSIXSLAVEPTY_MASK_SHFL 3

#define FLAG_NEW_CMD 255
#define FLAG_SEND_OUT 128
#define FLAG_TERM_MSG 135
#define FLAG_TERM_PTY 221
#define FLAG_TERM_MASTER 255

#define MASK_YIELD_FAILURE(YIELD, MASK, SHIFT) (((-1 * YIELD) << SHIFT) | MASK)
#define IS_YIELD_FAIL(...) ((_VA_ARGS_) < 0)

typedef long yield_t;

#endif