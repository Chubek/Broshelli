#ifndef __windows__

#include <windows.h>
#include <process.h>

typedef HANDLE winfdsc_t;
typedef HRESULT winyield_t;
typedef HPCON winconpty_t;



typedef struct WinConPty {
	winfdsc_t 
} conpty_t;

static inline winyield_t windows_open_conpty(winconpty_t *pty) {

}










#endif