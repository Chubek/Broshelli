#ifndef PTYDEF_H
#define PTYDEF_H

#ifndef 




typedef int ptydsc_t;
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