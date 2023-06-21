#ifndef MAX_CORE_SIZE
#define MAX_CORE_SIZE 100
#endif

#define TAG(CHAR) (CHAR == L'%')
#define VAL_TERM(CHAR) (CHAR == L',' || CHAR == L'}')
#define NEXT(STREAM) *STREAM++

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <wchar.h>
#include <sys/types.h>

typedef wchar_t btychar_t;
typedef wchar_t *btystr_t;
typedef uint64_t btyint_t;
typedef uint32_t btyptyid_t;
typedef double btyflt_t;
typedef uint32_t btymsglen_t;

typedef enum NativeMsgType {
	Command,
	Directive,
} btymsgtyy_t;

typedef union BtyNativeMessage {
	char arena[MAX_CORE_SIZE];
	struct {
		btyptyid_t ptyid;
		btymsgtyy_t type;
		btystr_t core;
		union {
			char diff[4];
			btymsglen_t whole;
		} length;
	} data;	
} btycontext_t;


static inline void allcoate_new_message_context(btycontext_t **context) {
	*context = calloc(1, sizeof(btycontext_t));
}

static inline void deallocate_message_context(btycontext_t **context) { 
	free(*context); 
}

static inline void reset_message_context(btycontext_t *context) {  
	memset(context, 0, sizeof(btycontext_t));  
}

static void read_wide_json(FILE *file, wchar_t *fmt, ...) {
	wchar_t chr, fchr;
	va_list argls;
	va_start(argls, fmt);
	while (fchr = NEXT(fmt)) {
		if (TAG(fchr)) {
			fchr = NEXT(fmt);
			switch(fchr) {
			case L'i':
				uint64_t num = 0, *numptr = va_arg(argls, uint64_t*);
				do {
					chr = fgetwc(file);
					if (VAL_TERM(chr))
						break;
					num = ((num << 3) + (num << 1)) + (chr - L'0');
				} while (1);
				*numptr = num;
				break;
			case L'b':
				char *bytes = va_arg(argls, char*);
				do {
					chr = fgetc(file);
					if (VAL_TERM(chr))
						break;
					*(bytes++) = chr;
				} while (1);
				break;
			case L's':
				wchar_t *wstring = va_arg(argls, wchar_t*);
				wchar_t oldchr;
				do {
					chr = fgetwc(file);
					if (VAL_TERM(chr) && oldchr == L'"')
						break;
					*(wstring++) = chr;
					oldchr = chr;
				} while (1);
				break;
			default:
				break;
		}
	} else {
		fgetwc(file);
	}
}
	va_end(argls);
}

int main() {
	btycontext_t *context;
	int ret;
	uint64_t tst;
	//allcoate_new_message_context(&context);
	read_wide_json(stdin, L"{%i}", &tst);
	//deallocate_message_context(&context);

}