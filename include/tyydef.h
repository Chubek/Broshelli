#ifndef TYYDEF_H
#define TYYDEF_H

typedef long yield_t;
typedef int fdsc_t;
typedef int ptydsc_t;
typedef int pdsc_t[2];
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