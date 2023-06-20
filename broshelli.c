#ifndef MAX_CORE_SIZE
#define MAX_CORE_SIZE 10000200
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wchar.h>
#include <sys/types.h>

typedef wchar_t btychar_t;
typedef wchar_t *btystr_t;
typedef uint64_t btyint_t;
typedef uint16_t btyptyid_t;
typedef double btyflt_t;
typedef uint32_t btymsglen_t;

typedef enum NativeMsgType {
	Command,
	Directive,
} btymsgtyy_t;

typedef struct BtyNativeMessage {
	btystr_t core[MAX_CORE_SIZE];
	btyptyid_t ptyid;
	btyymsgtyy_t type;
	btymsglen_t length;
} btycontext_t;


static inline void allcoate_new_message_arena(btycontext_t *context) {
	context = calloc(1, sizeof(btycontext_t));
}

static inline void deallocate_message_arena(btycontext_t *context) { 
	free(context); 
}

static inline void reset_message_context(btycontext_t *context) {  
	memset(context, 0, sizeof(btycontext_t));  
}

static inline void read_stdin_to_context(btycontext_t *context) {  
	fwscanf(stdin, "%u{\"type\":%d,\"core\":\"%s\",\"ptyid\":%hu}", context->length, context->type, context->core, context->ptyid);
}

static inline void write_context_to_stdout(btycontext_t *context) {
	fwprintf(stdout, "%u{\"core\":\"%s\"}", context->core);
}