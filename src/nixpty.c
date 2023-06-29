#include "dyndef.h"
#include "ptydef.h"
#include "valdef.h"
#include "iodef.h"

yield_t nixio_substitue_stdio(ptydsc_t subfdsc) {
	yield_t yieldval;
	ptydsc_t fdscstdin = dup(subfdsc);
	ptydsc_t fdscstdout = dup(subfdsc);
	ptydsc_t fdscstderr = dup(subfdsc);
	if (IS_YIELD_FAIL(yieldval = dup2(STDIN_FD, fdscstdin)))
		return MASK_YIELD_FAILURE(yieldval, FDSCDUP_FAIL_STDIN, FDSCDUP_MASK_SHFL);
	else if (IS_YIELD_FAIL(yieldval = dup2(STDOUT_FD, fdscstdout)))
		return MASK_YIELD_FAILURE(yieldval, FDSCDUP_FAIL_STDOUT, FDSCDUP_MASK_SHFL);
	else if (IS_YIELD_FAIL(yieldval = dup2(STDERR_FD, fdscstderr)))
		return MASK_YIELD_FAILURE(yieldval, FDSCDUP_FAIL_STDERR, FDSCDUP_MASK_SHFL);
	return FDSCDUP_SUCCESS_STDIO;
}

yield_t nixio_open_posix_pty_master(ptyfile_t *pty) {
	yield_t yieldval;
	if (IS_YIELD_FAIL(yieldval = pty->fdsc = posix_open(O_RDWR | O_NOCTTY)))
		return MASK_YIELD_FAILURE(yieldval, POSIXMASTERPTY_FAILURE_OPEN, POSIXMASTERPTY_MASK_SHFL);
	else if (IS_YIELD_FAIL(yieldval = grantpt(pty->fdsc)))
		return MASK_YIELD_FAILURE(yieldval, POSIXMASTERPTY_FAILURE_GRANT, POSIXMASTERPTY_MASK_SHFL);
	else if (IS_YIELD_FAIL(yieldval = unlockpt(pty->fdsc)))
		return return MASK_YIELD_FAILURE(yieldval, POSIXMASTERPTY_FAILURE_UNLOCK, POSIXMASTERPTY_MASK_SHFL);
	pty->fstream = fdopen(pty->fdsc, "w");
	return POSIXMASTERPTY_SUCCESS_OPEN;
}

yield_t nixio_open_posix_pty_slave(ptydsc_t masterdsc, ptyfile_t *pty) {
	yield_t yeidlval;
	if (IS_YIELD_FAIL(yieldval = ptsname_r(masterdsc, &pty->fname[0], PTY_NAME_LEN_MAX)))
		return MASK_YIELD_FAILURE(yieldval, POSIXSLAVEPTY_FAILURE_PTSNAME, POSIXSLAVEPTY_MASK_SHLF);
	else if (IS_YIELD_FAIL(yieldval = pty->fdsc = open(&pty->fname[0], O_RDWR | O_NOCTTY)))
		return MASK_YIELD_FAILURE(yieldval, POSIXSLAVEPTY_FAILURE_OPEN, POSIXSLAVEPTY_MASK_SHFL);
	pty->fstream = fdopen(pty->fdsc, "w");
	return POSIXSLAVEPTY_SUCCESS_OPEN;
}

