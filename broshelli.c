#include "broshelli.h"

int context_io_handle(btyctx_t *ctx, btydir_t dir) {
	FILE *tmpf;
	pid_t pid = getpid();
	char chr, fpath[TMPSTATEFPATH_LEN] = P_tmpdir, *fpaths = &fpath[0], *fpathscp = fpaths;
	while ((chr = *fpaths++));
	*fpaths++ = '/';
	sprintf(fpaths, "%d.tmp", pid);
	if (dir == STORE_CTX) {
		tmpf = fopen(fpathscp, "w");
		return fwrite(ctx, sizeof(*ctx), 1, tmpf);
	} else if (dir == RETRIEVE_CTX) {
		tmpf = fopen(fpathscp, "r");
		return fread(ctx, sizeof(*ctx), 1, tmpf);
	} else if (dir == REMOVE_CTX) {
		return unlink(fpathscp);
	}
}

void interrupt_signal_handler(int signum) {
	if (signum == SIGINT) {
		btyctx_t *ctx;
		context_io_handle(ctx, RETRIEVE_CTX);
		close(ctx->masterfd);
		close(ctx->slavefd);
		fclose(ctx->smaster);
		_exit(EXIT_SUCCESS);
	}
}

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
	if ((ctx->slavepid = fork()) < 0)
		return ctx->slavepid;
	if (!ctx->slavepid) {
		signal(SIGINT, interrupt_signal_handler);
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
		if ((ret = execlp(shell, shell, (char*)NULL)) < 0)
			_exit(EXIT_ERR);
		context_io_handle(ctx, STORE_CTX);
	} else {
		ctx->smaster = fdopen(ctx->masterfd, "w");
	}
}

int write_stream_to_master(btyctx_t *ctx, char *stream, char term) {
	char chr;
	while ((chr = fputc(*stream++, ctx->smaster)) != term);
}

int read_stream_from_master(btyctx_t *ctx, char *stream, char term) {
	char chr;
	while ((chr = *stream++ = fgetc(ctx->smaster)) != term);
}

void parse_browser_message(btyctx_t *ctx) {
	memset(ctx->browserio.arena, 0, sizeof(ctx->browserio.arena));
	fread(&ctx->browserio.msg.size, sizeof(uint32_t), 1, stdin);
	scanf("{\"ptyid\":%hu,\"cmd\":\"%s\"}", &ctx->browserio.msg.ptyid, ctx->browserio.msg.command);
}

void marshal_browser_message(btyctx_t *ctx) {
	fwrite(&ctx->browserio.msg.size, sizeof(uint32_t), 1, stdout);
	printf("{\"ptyid\":%hu,\"cmd\":\"%s\"}", ctx->browserio.msg.ptyid, ctx->browserio.msg.command);	
}

int main() {
	btyctx_t ctx;
	parse_browser_message(&ctx);
	marshal_browser_message(&ctx);
}