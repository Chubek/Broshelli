#ifndef IDTDEFS_H
#define IDTDEFS_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


typedef long idtid_t;
typedef int idtdsc_t;
typedef char idtname_t[IDT_NAME_LEN_MAX];

typedef struct IDTermIO {
	idtid_t tid;
	idtdsc_t fdsc;
	idtname_t fname;
} idtermio_t;








#endif