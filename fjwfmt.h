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

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDARG_H
#define STDARG_H
#include <stdarg.h>
#endif

#ifndef STDINT_H
#define STDINT_H
#include <stdint.h>
#endif

#ifndef WCHAR_H
#define WCHAR_H
#include <wchar.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
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

static void fjscanws(wchar_t *str, wchar_t *fmt, ...) {
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
          chr = NEXT(str);
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
          chr = NEXT(str);
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
        memmove(bytesptr, str, numbytes);
        str += numbytes;
        break;
      case L's':
        wchar_t **wstrarg = va_arg(argls, wchar_t **),
                          *wstr = *wstrarg;
        NEXT(str);
        do {
          chr = NEXT(str);
          if (STR_TERM(chr, wprevchr))
            break;
          NEXT_DEREF(wstr) = chr;
          wprevchr = chr;
        } while (1);
        break;
      case L'a':
        char **astrarg = va_arg(argls, char **),
                             *astr = *astrarg;
        NEXT(str);
        do {
          chr = NEXT(str);
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
      NEXT(str);
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


static void fjprintws(wchar_t *str, wchar_t *fmt, ...) {
  wchar_t chr, fchr;
  va_list argls;
  va_start(argls, fmt);
  while ((fchr = NEXT(fmt))) {
    if (TAG(fchr)) {
      fchr = NEXT(fmt);
      switch (fchr) {
      case L'i':
        uint64_t intptr = va_arg(argls, uint64_t);
        swprintf(str, sizeof(uint64_t) * 2, L"%lu", intptr);
        break;
      case L'f':
        double fltptr = va_arg(argls, double);
        swprintf(str, sizeof(uint64_t) * 2, L"%f", fltptr);
        break;
      case L'b':
        chr = NEXT(fmt);
        int numbytes = (int)(fchr - L'0'), nextbyte = 0;
        char *bytesptr = va_arg(argls, char *);
        memmove(str, bytesptr, numbytes);
        str += numbytes;
        break;
      case L's':
        wchar_t *wstr = va_arg(argls, wchar_t *);
        NEXT_DEREF(str) = L'"';
        wcscpy(str, wstr);
        NEXT_DEREF(str) = L'"';
        break;
      case L'a':
        char achr, *astr = va_arg(argls, char *);
        NEXT_DEREF(str) = L'"';
        while ((achr = *astr++))
      	NEXT_DEREF(str) = wctob(achr);
        NEXT_DEREF(str) = L'"';
        break;
      default:
        break;
      }
    } else {
      NEXT_DEREF(str) = fchr;
    }
  }
  va_end(argls);
}

#define jscanwf(FMT, ...) fjscanwf(stdin, FMT, __VA_ARGS__)
#define jprintwf(FMT, ...) fjprintwf(stdout, FMT, __VA_ARGS__)
#define jscanf(FMT, ...) fjscanf(stdin, FMT, __VA_ARGS__)
#define jprintf(FMT, ...) fjprintf(stdout, FMT, __VA_ARGS__)


#endif