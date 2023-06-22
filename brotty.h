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

#ifndef SPTYNAME_MAX
#define SPTYNAME_MAX 64
#endif

typedef struct {
	int master;
	int slave;
	char slavename[SPTYNAME_MAX];
	FILE *mstream;
	FILE *sstream;
} bpty_t;

static inline open_pty_device_pair(bpty_t *pty) {
	pty->master = posix_openpt(ORDWR | ONOCTTY);
	#ifndef __NO_GRANT__
	grantpt(pty->master);
	#endif
	unlockpt(pty->master);
	char *sname = &pty->slavename[0];
	sname = ptsname(pty->master);
	pty->slave = open(sname, O_RDONLY);
	pty->mstream = fdopen(pty->master);
	pty->sstream = fdopen(pty->slave);

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