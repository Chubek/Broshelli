#ifndef TYYDEF_H
#define TYYDEF_H


typedef struct PtyPair {
  int masterfd;
  int slavefd;
  char slavefn[SLVFNAME_LEN_MAX];
  pid_t slavepid;
  FILE *masterfstm;
} ptypair_t;





#endif