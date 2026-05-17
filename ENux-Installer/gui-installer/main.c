/*
 * main.c -- main()
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "gui.h"

bool
checkroot(void)
{
	return geteuid() == 0;
}

bool
hasinet(void)
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		/* we don't really know if the user has internet, but if he/she can't make a
		 * socket then ugh... no */
		return false;
	}

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(53); /* dns port */
	if (inet_pton(AF_INET, "8.8.8.8", &addr.sin_addr) <= 0) {
		close(sockfd);
		return 0;
	}

	int res = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	close(sockfd);
	return (res == 0);
}

int
main(int argc, char** argv)
{
	(void)argc; (void)argv;

	if (!checkroot()) {
		fprintf(stderr, "You must run this program as root.\n");
		return EXIT_FAILURE;
	}

	if (!hasinet()) {
		fprintf(stderr, "No internet.\n");
		return EXIT_FAILURE;
	}

	GUI* ui = setupUI();
	if (!ui)
		return EXIT_FAILURE;

	runUI();
	teardownUI(ui);

	return EXIT_SUCCESS;
}

