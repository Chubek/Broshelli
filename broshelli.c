#ifndef MAX_CORE_SIZE
#define MAX_CORE_SIZE 100
#endif

#ifndef FDIGIT_MAX
#define FDIGIT_MAX 256
#endif

#ifndef IDIGIT_MAX
#define IDIGIT_MAX 25
#endif

#ifndef BYTES_MAX
#define BYTES_MAX 10
#endif

#define TAG(CHAR) (CHAR == L'%')
#define VAL_TERM(CHAR) (CHAR == L',' || CHAR == L'}')
#define VAL_TERM_ASCII(CHAR) (CHAR == ',' || CHAR == '}')
#define STR_TERM(CHAR, PREV_CHAR) (VAL_TERM(CHAR) && (PREV_CHAR == L'"'))
#define STR_TERM_ASCII(CHAR, PREV_CHAR) (VAL_TERM_ASCII(CHAR) && (PREV_CHAR == '"'))
#define NEXT(STREAM) *STREAM++
#define NEXT_DEREF(STREAM) *(STREAM++)
#define IPARSE(NUM, CHAR) ((NUM << 3) + (NUM << 1)) + (CHAR - L'0')


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

static void parse_formatted_json(FILE *file, wchar_t *fmt, ...) {
	wchar_t chr, fchr;
	va_list argls;
	va_start(argls, fmt);
	while (fchr = NEXT(fmt)) {
		if (TAG(fchr)) {
			fchr = NEXT(fmt);
			switch(fchr) {
			case L'i':
				uint64_t *intptr = va_arg(argls, uint64_t*);
				wchar_t idigits[IDIGIT_MAX] = {0}, *idptr = &idigits[0];
				do {
					chr = fgetwc(file);
					if (VAL_TERM(chr))
						break;
					NEXT_DEREF(idptr) = chr;
				} while (1);
				*intptr = wcstol(idigits, NULL, 10);
				break;
			case L'f':
				double *fltptr = va_arg(argls, double*);
				wchar_t fdigits[FDIGIT_MAX] = {0}, *fdptr = &fdigits[0];
				do {
					chr = fgetwc(file);
					if (VAL_TERM(chr))
						break;
					NEXT_DEREF(fdptr) = chr;
				} while (1);
				*fltptr = wcstod(fdigits, NULL);
				break;
			case L'b':
				fchr = NEXT(fmt);
				int numbytes = (int)(fchr - L'0'), nextbyte = 0;
				char *bytesptr = va_arg(argls, char*);
				do {
					bytesptr[nextbyte++] = fgetc(file);
				} while (--numbytes);
				break;
			case L's':
				wchar_t **wstrarg = va_arg(argls, wchar_t**), *wstr = *wstrarg;
				wchar_t wprevchr;
				do {
					chr = fgetwc(file);
					if (STR_TERM(chr, wprevchr))
						break;
					NEXT_DEREF(wstr) = chr;
					wprevchr = chr;
				} while (1);
				break;
			case 'a':
				char **astrarg = va_arg(argls, char**), *astr = *astrarg;
				char aprevchr;
				do {
					chr = fgetc(file);
					if (STR_TERM_ASCII(chr, aprevchr))
						break;
					NEXT_DEREF(astr) = chr;
					aprevchr = chr;
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

static void marshal_formatted_json(FILE *file, wchar_t *fmt, ...) {
	wchar_t chr, fchr;
	va_list argls;
	va_start(argls, fmt);
	while (fchr = NEXT(fmt)) {
		if (TAG(fchr)) {
			fchr = NEXT(fmt);
			switch(fchr) {
			case L'i':
				uint64_t *intptr = va_arg(argls, uint64_t*);
				fwprintf(file, L"%lu", *intptr);
				break;
			case L'f':
				double *fltptr = va_arg(argls, double*);
				fwprintf(file, L"%f", *fltptr);
				break;
			case L'b':
				chr = NEXT(fmt);
				int numbytes = (int)(fchr - L'0'), nextbyte = 0;
				char *bytesptr = va_arg(argls, char*);
				do {
					fputc(bytesptr[nextbyte++], file);
				} while (--numbytes);
				break;
			case L's':
				wchar_t **wstrarg = va_arg(argls, wchar_t**), *wstr = *wstrarg;
				do {
					chr = *wstr++;
					fputwc(chr, file);
					if (!chr)
						break;
				} while (1);
				break;
			case 'a':
				char **astrarg = va_arg(argls, char**), *astr = *astrarg;
				do {
					chr = *astr++;
					fputc(chr, file);
					if (!chr)
						break;
				} while (1);
				break;
			default:
				break;
		}
	} else {
		fputwc(fchr, file);
	}
}
	va_end(argls);
}

int main() {
	btycontext_t *context;
	int ret;
	uint64_t tst;
	//allcoate_new_message_context(&context);
	parse_formatted_json(stdin, L"{'ZZ':%i}", &tst);
	marshal_formatted_json(stdout, L"{'KK':%i}", &tst);
	//deallocate_message_context(&context);

}