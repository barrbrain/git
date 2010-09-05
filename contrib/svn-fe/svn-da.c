/*
 * This file is in the public domain.
 * You may freely use, modify, distribute, and relicense it.
 */

#include <stdio.h>
#include "svndiff.h"

int main(int argc, char **argv)
{
	struct svndiff_window *window;
	FILE *source;

	if (argc != 2)
		return 1;
	
	svndiff_init();
	source = fopen(argv[1], "r");
	if (!source)
		return 1;

	read_header();

	window = malloc(sizeof(*window));
	drive_window(window, source);
	free(window);

	svndiff_deinit();
	return 0;
}
