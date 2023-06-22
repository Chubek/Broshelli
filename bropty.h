#ifndef BROTY_H
#define BROTY_H


#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif


#ifndef FCNTL_H
#define FCNTL_H
#include <fcntl.h>
#endif

#ifndef UNISTD_H
#define UNISTD_H
#include <unistd.h>
#endif

#ifndef STDINT_H
#define STDINT_H
#include <stdint.h>
#endif

#ifndef SPTYNAME_LEN
#define SPTYNAME_LEN 64
#endif

typedef struct {
	int masterfd;
	int slavefd;
	char slavefdname[SPTYNAME_LEN];
	FILE *masters;
	FILE *slaves;
} bpty_t;


int open_pty_device_pair(bpty_t *pty);
void close_pty_device_pair(bpty_t *pty);
char write_to_master(bpty_t *pty, char *data, char term, size_t maxlen);
char read_from_slave(bpty_t *pty, char *data, char term, size_t maxlen);

#endif