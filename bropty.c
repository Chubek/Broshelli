#include "bropty.h"


int open_pty_device_pair(bpty_t *pty) {
	int ret;
	if ((ret = pty->masterfd = posix_openpt(ORDWR | ONOCTTY)) < 0)
		return ret;
	#ifndef NO_GRANT
	if ((ret = grantpt(pty->masterfd)) < 0)
		return ret;
	#endif
	if ((ret = unlockpt(pty->masterfd)))
		return ret;
	char *sname = &pty->slavefdname[0];
	if ((ret = sname = ptsname(pty->masterfd)) < 0) 
		return ret;
	if ((ret = pty->slavefd = open(sname, O_RDONLY)) < 0)
		return ret;
	pty->masters = fdopen(pty->masterfd, "w");
	pty->slaves = fdopen(pty->slavefd, "w");
}

void close_pty_device_pair(bpty_t *pty) {
	close(pty->masterfd);
	close(pty->slavefd);
	fclose(pty->masters);
	fclose(pty->slaves);
}

char write_to_master(bpty_t *pty, char *data, char term, size_t maxlen) {
	char chr;
	while (((chr = fputc(*dst++, pty->masters)) != term) && --maxlen);
	return chr
}

char read_from_slave(bpty_t *pty, char *data, char term, size_t maxlen) {
	char chr;
	while ((chr = (*(data++) = fgetc(pty->slaves)) != term) && --maxlen);
	return chr;
}