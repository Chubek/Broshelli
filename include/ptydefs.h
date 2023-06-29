#ifndef PTYDEFS_H
#define PTYDEFS_H

#define _XOPEN_SOURCE  600
#define _GNU_SOURCE
#include <stdlib.h>
#include <stio.h>
#include <unistd.h>

typedef int trialnum_t;
typedef int ptydsc_t;
typedef char ptychr_t;
typedef char ptyname_t[PTY_NAME_LEN_MAX];


typedef struct PseudoTerminal {
  ptydsc_t fdsc;
  ptyname_t fname;
  FILE *fstream;
} ptyfile_t;

typedef struct PseudoTerminalPair {
  ptyfile_t masterpty;
  ptyfile_t slavepty;
} ptypair_t;


#endif