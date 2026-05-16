/*
 * main.c -- main()
 */

#include <stdlib.h>

#include "gui.h"

int
main(int argc, char** argv)
{
	(void)argc; (void)argv;

	App* ui = setupUI();
	if (!ui)
		return EXIT_FAILURE;

	runUI();
	teardownUI(ui);

	return EXIT_SUCCESS;
}

