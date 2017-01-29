/**
 * entropy.c - simple entropy calculator
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <squish/entropy.h>
#include <version.h>

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "entropy version %s\n"
		                "calculates entropy of a file\n\n"
		                "usage: %s <filename>\n",
		        VERSION, argv[0]);
		return 1;
	}

	return 0;
}
