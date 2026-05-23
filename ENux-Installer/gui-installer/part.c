/*
 * part.c -- some partitioning stuff
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "run.h"
#include "part.h"

void
getSelectedDiskName(GUI* app, char* out, size_t outsz)
{
	int idx = uiComboboxSelected(app->diskSelect);

	if (idx < 0 || idx >= app->diskLabelCount || app->diskLabelCount == 0) {
		out[0] = '\0';
		return;
	}

	const char* label = app->diskLabels[idx];

	const char* start = strstr(label, "/dev/");
	if (!start) {
		out[0] = '\0';
		return;
	}

	const char* end = strchr(start, ' ');
	if (!end)
		end = start + strlen(start);

	size_t len = (size_t)(end - start);
	if (len >= outsz) len = outsz - 1;

	strncpy(out, start, len);
	out[len] = '\0';
}


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

