/*
 * main.c -- main()
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gui.h"

bool
checkroot(void)
{
	return geteuid() == 0;
}

int
main(int argc, char** argv)
{
	(void)argc; (void)argv;

	if (!checkroot()) {
		fprintf(stderr, "You must run this program as root.\n");
		return EXIT_FAILURE;
	}

	GUI* ui = setupUI();
	if (!ui)
		return EXIT_FAILURE;

	runUI();
	teardownUI(ui);

	return EXIT_SUCCESS;
}

