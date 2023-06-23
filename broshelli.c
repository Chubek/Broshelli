#include "fjwfmt.h"
#include "broshelli.h"


int open_pty_pair(btyctx_t *ctx) {
	int ret;
	if ((ret = ctx->masterfd = posix_openpt(O_RDWR | O_NOCTTY)) < 0)
		return ret;
	if ((ret = unlockpt(ctx->masterfd)) < 0)
		return ret;
	if ((ret = grantpt(ctx->masterfd)) < 0)
		return ret;
	if ((ret = ptsname_r(ctx->masterfd, &ctx->slavefname[0], SLVFN_LEN)) < 0)
		return ret;
	if ((ret = ctx->slavefd = open(ctx->slavefname, O_RDWR | O_NOCTTY)) < 0)
		return ret;
}

int fork_off_slave_and_exec(btyctx_t *ctx) {
	if ((ret = ctx->slavepid = fork()) < 0)
		return ret;
	if (!ctx->slavepid) {
		char *shell;
		int slvfdcp1, slvfdcp2, slvfdcp3, ret;
		slvfdcp1 = dup(ctx->slavefd);
		slvfdcp2 = dup(ctx->slavefd);
		slvfdcp3 = dup(ctx->slavefd);
		if ((ret = dup2(STDIN_FD, slvfdcp1)) < 0)
			_exit(EXIT_ERR);
		if ((ret = dup2(STDOUT_FD, slvfdcp2)) < 0)
			_exit(EXIT_ERR);
		if ((ret = dup2(STDERR_FD, slvfdcp3)) < 0)
			_exit(EXIT_ERR);
		if (!(shell = getenv(SHELL_ENV)))
			shell = SHELL_DFL;
		if ((ret = EXECV_FN(shell, EXEC_ARGS)) < 0)
			_exit(EXIT_ERR);
	}
}

