/*

fjwfmt.h: Formatted JSON I/O
License Info: 2023 - Chubak Bidpaa. This application programming interface is released under free license. You are free to do with the code as
you wish. However, the author is NOT responsible for whatever shortcomins this application may have. There is no guarantee that this application
will work as intended. If you use this application in a production environment, you are to be held responsible for however it acts.

  This header file contains 8 functions and 4 macros which all operate in a similar
fashion. They all accept a file or a string, a format string, and an invariable
number of arguments. The aim is to input/output the JSON in file/string (the first argument)
to the variable arguments (arguments after fmt) based on tags mentioned in fmt.

  In a way, these functions operate similar to all the `formatted I/O` functions of `stdio.h`,
except with some quircks  which make them useful to me personally. For example, if you specifigy an `%s`
tag and pass a string, it won't output the extra characters in the bufffer like `scanf` does, making it useful
for arena allocation. And in fact, for the strings at least, tht is how I recommend you use them: wit arena allocation.
Mainly because NONE OF THESE FUNCTIONS ALLOCATE MEMORY. It is expected of you to provide memory, and thusly I recommend
arena allocation. 

  Another quirck of these functions are that all the single quotes in `fmt` are treated as double quotes. As for the scan
functions, basically you can input any character, as long as that single character corresponds with a single character in the 
input JSON. However, as for print functions, any single quote in the fmt is treated as a double quote when output. This will 
help the programmer not deal with escaping the double quotes.

  There are total of eight functions in this file;

* For formatted parsing:
** fjscanf(FILE *file, char *fmt, ...)
** fjscwns(char *jstr, char *fmt, ...)
** fjscanwf(FILE *file, wchar_t *fmt, ...)
** fjscanws(wchar_t *jstr, wchar_t *fmt, ...)

* For formatted marshalling:
** fjprintf(FILE *file, char *fmt, ...)
** fjprints(char *jstr, char *fmt, ...)
** fjprintwf(FILE *file, wchar_t *fmt, ...)
** fjprintws(wchar_t *jstr, wchar_t *fmt, ...)

  Functions that have `w` on them operate on wide characters (Unicode).

* And four macros:

** jscanf(FMT, ...)
** jscanwf(FMT, ...)
** jprintf(FMT, ...)
** jprintwf(FMT, ...)

  As you may have guessed, the four macros use STDIN and STDOUT as their file. They are basically a call to the functions with the first
argument set to `stdin` or `stdout`.

  The format string accepts 4 directives. These directives begin with `%`. You can escape this with another `%`. 
These directives are:

* %i -> integer
* %f -> float
* %s -> string
* %bn -> n raw bytes, where n is a single digit between 1 and 9.

  Now, the wide character functions accept another directive:

* a -> ASCII strings

  Every variable argument to the `scan` function family must be a pointer. The integer and floats passed to print function family must NOT be a pointer.

Example:

```
uint64_t int;
double flt;
char str[10];

jscanf("'integerKey':%i,'floatKey':%f,'strKey':%s", &int, %flt, &str[0]); // single quotes treated as double quotes
jprintf("'strKey':%s,'integerKey':%i,'floatKey':%f", &str[0]); // single quotes become double quotes

```

  You need to prefix your format string to wide-character function family with `L` as in:

```
wchar_t str[10];
jscanwf(L"'keyStr':%s", &str[0]);
```

Please contact me if you have any questions:

chubakbidpaa@gmail.com
Discord: .chubak

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <wchar.h>
#include <string.h>


#ifndef FJWFMT_H
#define FJWFMT_H

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
#define ATAG(CHAR) (CHAR == '%')
#define VAL_TERM(CHAR) (CHAR == L',' || CHAR == L'}')
#define STR_TERM(CHAR, PREV_CHAR) (CHAR == L'"' && PREV_CHAR != L'\\')
#define NEXT(STREAM) *STREAM++
#define NEXT_DEREF(STREAM) *(STREAM++)

static void fjscanf(FILE *file, char *fmt, ...) {
  char chr, fchr, prevchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case 'i':
        uint64_t *intptr = va_arg(argls, uint64_t *);
        char idigits[IDIGIT_MAX] = {0}, *idptr = &idigits[0];
        do {
          chr = fgetc(file);
          if (VAL_TERM(chr))
            break;
          NEXT_DEREF(idptr) = chr;
        } while (1);
        *intptr = strtoul(idigits, NULL, 10);
        break;
      case 'f':
        double *fltptr = va_arg(argls, double *);
        char fdigits[FDIGIT_MAX] = {0}, *fdptr = &fdigits[0];
        do {
          chr = fgetc(file);
          if (VAL_TERM(chr))
            break;
          NEXT_DEREF(fdptr) = chr;
        } while (1);
        *fltptr = strtod(fdigits, NULL);
        break;
      case 'b':
        fchr = NEXT(fmt);
        int numbytes = (int)(fchr - L'0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        fread(bytesptr, 1, numbytes, file);
        break;
      case 's':
        char *str = va_arg(argls, char *);
        do {
          chr = fgetc(file);
          if (STR_TERM(chr, prevchr))
            break;
          NEXT_DEREF(str) = chr;
          prevchr = chr;
        } while (1);
        break;
      default:
        break;
      }
    } else {
      fgetc(file);
    }
  }
  va_end(argls);
}

static void fjscans(char *jstr, char *fmt, ...) {
  char chr, fchr, prevchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case 'i':
        uint64_t *intptr = va_arg(argls, uint64_t *);
        char idigits[IDIGIT_MAX] = {0}, *idptr = &idigits[0];
        do {
          chr = NEXT(jstr);
          if (VAL_TERM(chr))
            break;
          NEXT_DEREF(idptr) = chr;
        } while (1);
        *intptr = strtoul(idigits, NULL, 10);
        break;
      case 'f':
        double *fltptr = va_arg(argls, double *);
        char fdigits[FDIGIT_MAX] = {0}, *fdptr = &fdigits[0];
        do {
          chr = NEXT(jstr);
          if (VAL_TERM(chr))
            break;
          NEXT_DEREF(fdptr) = chr;
        } while (1);
        *fltptr = strtod(fdigits, NULL);
        break;
      case 'b':
        fchr = NEXT(fmt);
        int numbytes = (int)(fchr - L'0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        memmove(bytesptr, jstr, numbytes);
        jstr += numbytes;
        break;
      case 's':
        char *str = va_arg(argls, char *);
        do {
          chr = NEXT(jstr);
          if (STR_TERM(chr, prevchr))
            break;
          NEXT_DEREF(str) = chr;
          prevchr = chr;
        } while (1);
        break;
      default:
        break;
      }
    } else {
      NEXT(str);
    }
  }
  va_end(argls);
}

static void fjscanwf(FILE *file, wchar_t *fmt, ...) {
  wchar_t chr, fchr, wprevchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case L'i':
        uint64_t *intptr = va_arg(argls, uint64_t *);
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
        double *fltptr = va_arg(argls, double *);
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
        char *bytesptr = va_arg(argls, char *);
        fread(bytesptr, 1, numbytes, file);
        break;
      case L's':
        wchar_t **wstrarg = va_arg(argls, wchar_t **),
                           *wstr = *wstrarg;
        fgetwc(file);
        do {
          chr = fgetwc(file);
          if (STR_TERM(chr, wprevchr))
            break;
          NEXT_DEREF(wstr) = chr;
          wprevchr = chr;
        } while (1);
        break;
      case L'a':
        char **astrarg = va_arg(argls, char **),
                             *astr = *astrarg;
        fgetwc(file);
        do {
          chr = fgetwc(file);
          if (STR_TERM(chr, wprevchr))
            break;
          NEXT_DEREF(astr) = wctob(chr);
          wprevchr = chr;
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


static void fjprintf(FILE *file, char *fmt, ...) {
  char chr, fchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case 'i':
        uint64_t intptr = va_arg(argls, uint64_t);
        fprintf(file, "%lu", intptr);
        break;
      case 'f':
        double fltptr = va_arg(argls, double);
        fprintf(file, "%f", fltptr);
        break;
      case 'b':
        chr = NEXT(fmt);
        int numbytes = (int)(fchr - '0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        fwrite(bytesptr, 1, numbytes, file);
        break;
      case 's':
        char *str = va_arg(argls, char *);
        fputc('"', file);
        fputs(str, file);
        fputc('"', file);
        break;
      default:
        break;
      }
    } else {
      fputc((fchr == '\'') ? '"' : fchr, file);
    }
  }
  va_end(argls);
}

static void fjscanws(wchar_t *jjstr, wchar_t *fmt, ...) {
  wchar_t chr, fchr, wprevchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case L'i':
        uint64_t *intptr = va_arg(argls, uint64_t *);
        wchar_t idigits[IDIGIT_MAX] = {0}, *idptr = &idigits[0];
        do {
          chr = NEXT(jstr);
          if (VAL_TERM(chr))
            break;
          NEXT_DEREF(idptr) = chr;
        } while (1);
        *intptr = wcstol(idigits, NULL, 10);
        break;
      case L'f':
        double *fltptr = va_arg(argls, double *);
        wchar_t fdigits[FDIGIT_MAX] = {0}, *fdptr = &fdigits[0];
        do {
          chr = NEXT(jstr);
          if (VAL_TERM(chr))
            break;
          NEXT_DEREF(fdptr) = chr;
        } while (1);
        *fltptr = wcstod(fdigits, NULL);
        break;
      case L'b':
        fchr = NEXT(fmt);
        int numbytes = (int)(fchr - L'0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        memmove(bytesptr, jstr, numbytes);
        jstr += numbytes;
        break;
      case L's':
        wchar_t **wjstrarg = va_arg(argls, wchar_t **),
                          *wjstr = *wjstrarg;
        NEXT(jstr);
        do {
          chr = NEXT(jstr);
          if (jstr_TERM(chr, wprevchr))
            break;
          NEXT_DEREF(wjstr) = chr;
          wprevchr = chr;
        } while (1);
        break;
      case L'a':
        char **ajstrarg = va_arg(argls, char **),
                             *ajstr = *ajstrarg;
        NEXT(jstr);
        do {
          chr = NEXT(jstr);
          if (jstr_TERM(chr, wprevchr))
            break;
          NEXT_DEREF(ajstr) = wctob(chr);
          wprevchr = chr;
        } while (1);
        break;
      default:
        break;
      }
    } else {
      NEXT(jstr);
    }
  }
  va_end(argls);
}


static void fjprints(char *jstr, char *fmt, ...) {
  char chr, fchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case 'i':
        uint64_t intptr = va_arg(argls, uint64_t);
        sprintf(jstr, "%lu", intptr);
        while (chr) chr = NEXT(jstr);
        break;
      case 'f':
        double fltptr = va_arg(argls, double);
        sprintf(jstr, "%f", fltptr);
        while (chr) chr = NEXT(jstr);
        break;
      case 'b':
        chr = NEXT(fmt);
        int numbytes = (int)(fchr - '0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        memmove(jstr, bytesptr, numbytes);
        jstr += numbytes;
        break;
      case 's':
        char *str = va_arg(argls, char *);
        sprintf(jstr, "\"%s\"", jstr);
        while (chr) chr = NEXT(jstr);
        break;
      default:
        break;
      }
    } else {
      NEXT_DEREF(jstr) = (fchr == '\'') ? '"' : fchr;
    }
  }
  va_end(argls);
}

static void fjprintwf(FILE *file, wchar_t *fmt, ...) {
  wchar_t chr, fchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case L'i':
        uint64_t intptr = va_arg(argls, uint64_t);
        fwprintf(file, L"%lu", intptr);
        break;
      case L'f':
        double fltptr = va_arg(argls, double);
        fwprintf(file, L"%f", fltptr);
        break;
      case L'b':
        chr = NEXT(fmt);
        int numbytes = (int)(fchr - L'0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        fwrite(bytesptr, 1, numbytes, file);
        break;
      case L's':
        wchar_t *wstr = va_arg(argls, wchar_t *);
        fputwc(L'"', file);
        fputws(wstr, file);
        fputwc(L'"', file);
        break;
      case L'a':
        char achr, *astr = va_arg(argls, char *);
        fputwc(L'"', file);
        while ((achr = *astr++))
          fputwc(btowc(achr), file);
        fputwc(L'"', file);
        break;
      default:
        break;
      }
    } else {
      fputwc((fchr == L'\'') ? L'"' : fchr, file);
    }
  }
  va_end(argls);
}


static void fjprintws(wchar_t *jstr, wchar_t *fmt, ...) {
  wchar_t chr, fchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case L'i':
        uint64_t intptr = va_arg(argls, uint64_t);
        swprintf(jstr, sizeof(uint64_t) * 2, L"%lu", intptr);
        break;
      case L'f':
        double fltptr = va_arg(argls, double);
        swprintf(jstr, sizeof(uint64_t) * 2, L"%f", fltptr);
        break;
      case L'b':
        chr = NEXT(fmt);
        int numbytes = (int)(fchr - L'0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        memmove(jstr, bytesptr, numbytes);
        jstr += numbytes;
        break;
      case L's':
        wchar_t *wjstr = va_arg(argls, wchar_t *);
        NEXT_DEREF(jstr) = L'"';
        wcscpy(jstr, wjstr);
        NEXT_DEREF(jstr) = L'"';
        break;
      case L'a':
        char achr, *ajstr = va_arg(argls, char *);
        NEXT_DEREF(jstr) = L'"';
        while ((achr = *ajstr++))
        NEXT_DEREF(jstr) = wctob(achr);
        NEXT_DEREF(jstr) = L'"';
        break;
      default:
        break;
      }
    } else {
      NEXT_DEREF(jstr) = fchr;
    }
  }
  va_end(argls);
}

#define jscanwf(FMT, ...) fjscanwf(stdin, FMT, __VA_ARGS__)
#define jprintwf(FMT, ...) fjprintwf(stdout, FMT, __VA_ARGS__)
#define jscanf(FMT, ...) fjscanf(stdin, FMT, __VA_ARGS__)
#define jprintf(FMT, ...) fjprintf(stdout, FMT, __VA_ARGS__)


#endif