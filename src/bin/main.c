#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#include "config.h"
/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

char *filename = NULL;
char daemon_mode = 0;
int exit_flag = 0;
FILE *fp;

void usage(int argc, char *argv[])
{
	I("Usage: %s \n", argv[0]);
	I("     -b [destination base directory]\n");
	I("     -d [dry run]\n");
	I("     -k [keep run background]\n");
	I("     -f [manifest.xml filename], not empty\n");
	return;
}

int init_daemon(void)
{
	pid_t pid;
	int i;
	if ((pid = fork()) < 0)
		return -1;
	else if (pid != 0)
		exit(0);

	setsid();
	for (i=0; i < 3; ++i)
		close(i);
	umask(0);
	return 0;
}

void sig_term(int signo)
{
	if (signo == SIGTERM) {
		exit_flag = 1;
		I("catch signal(%d)\n", signo);
	}
}

int main(int argc, char *argv[])
{
	char *buf;
	size_t count;
	int ret = 0;

	signal(SIGTERM, sig_term);
	elink_alogrithm_init();
	elink_log_init();

	elink_rparse_data();
	if (daemon_mode) {
		openlog(argv[0], LOG_NOWAIT, LOG_DAEMON);
		init_daemon();
	}
	I("%s version: %s.%s\n", argv[0], MAJOR, MINOR);

	if (!filename) {
		usage(argc, argv);
		ret = -1;
		goto out;
	}

	if (!(fp = fopen(filename,"rb"))) {
		E("faile open file: %s\n", filename);
		usage(argc, argv);
		goto out;
	}
	count = 0;
	buf = malloc(SIZE);

	free(buf);
	buf = NULL;
	fclose(fp);

out:
	elink_log_shutdown();
	elink_alogrithm_shutdown();
	return ret;
}
