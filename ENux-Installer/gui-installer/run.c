/*
 * run.c -- run a command
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void
runcmd(const char* cmd)
{
	int ret = system(cmd);
	if (ret != 0) {
		fprintf(stderr, "command failed: %s\n", cmd);
		exit(1);
	}
}

