/*
 * part.c -- some partitioning stuff
 */

#include <stdio.h>
#include <stdlib.h>

#include "part.h"

Part*
getparts(size_t* count)
{
	FILE* fp = fopen("/proc/partitions", "r");
	if (!fp) {
		perror("fopen");
		return NULL;
	}

	char line[256];
	fgets(line, sizeof(line), fp); /* skip first line */
	fgets(line, sizeof(line), fp); /* and second too */

	size_t used = 0;
	size_t cap = 16;
	Part* parts = malloc(cap * sizeof(Part));
	if (!parts) {
		fclose(fp);
		return NULL;
	}

	while (fgets(line, sizeof(line), fp)) {
		Part p;
		int f = sscanf(
			line,
			"%d %d %llu %63s",
			&p.major,
			&p.minor,
			&p.blocks,
			p.name
		);

		if (f != 4)
			continue;

		if (used >= cap) {
			cap *= 2;
			Part* tmp = realloc(parts, cap * sizeof(Part));

			if (!tmp) {
				free(parts);
				fclose(fp);
				return NULL;
			}

			parts = tmp;
		}

		parts[used++] = p;
	}

	fclose(fp);
	*count = used;
	return parts;
}

