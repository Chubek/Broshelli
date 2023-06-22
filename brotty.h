#ifndef BROTY_H
#define BROTY_H


#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif


#ifndef PTY_H
#define PTY_H
#include <pty.h>
#endif

#ifndef UNISTD_H
#define UNISTD_H
#include <unistd.h>
#endif

#ifndef STDINT_H
#define STDINT_H
#include <stdint.h>
#endif

typedef struct {
	int master;
	int slave;
	FILE *mstream;
	FILE *sstream;
} btypty_t;

static inline open_pty_device(btypty_t *pty) {
	openpty(&pty->master, &pty->slave, NULL, NULL);
	pty->mstream = fdopen(pty->master, "w");
	pty->sstream = fdopen(pty->slave, "w");
}

static inline write_to_master(btypty_t *pty, char *data, char term) {
	char chr;
	while ((chr = fputc(*dst++, pty->mstream)) != term);
	return chr;
}

static inline read_from_slave(btypty_t *pty, char *data, char term) {
	char chr;
	while (chr = (*(data++) = fgetc(pty->sstream)) != term);
	return chr;
}


#endif