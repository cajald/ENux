/*
 * run.c -- run a command
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void
runcmd(const char* fmt, ...)
{
	char cmd[1024];

	va_list ap;
	va_start(ap, fmt);

	vsnprintf(cmd, sizeof(cmd), fmt, ap);

	va_end(ap);

	int ret = system(cmd);

	if (ret != 0) {
		fprintf(stderr, "command failed: %s\n", cmd);
		exit(1);
	}
}
