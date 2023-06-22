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
} bpty_t;

static inline open_pty_device_pair(bpty_t *pty) {
	openpty(&pty->master, &pty->slave, NULL, NULL);
	pty->mstream = fdopen(pty->master, "w");
	pty->sstream = fdopen(pty->slave, "w");
}

static inline close_pty_device_pair(bpty_t *pty) {
	close(pty->master);
	close(pty->slave);
}

static inline write_to_master(bpty_t *pty, char *data, char term, size_t maxlen) {
	char chr;
	while (((chr = fputc(*dst++, pty->mstream)) != term) && --maxlen);
	return chr;
}

static inline read_from_slave(bpty_t *pty, char *data, char term, size_t maxlen) {
	char chr;
	while ((chr = (*(data++) = fgetc(pty->sstream)) != term) && --maxlen);
	return chr;
}

#endif