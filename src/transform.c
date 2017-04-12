/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "image.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))
#define BLOCKSIZE 16
#define NUMBLOCKS (ARRAY_SIZE(data_image_raw) / BLOCKSIZE)

#define pixel(block, offset) ((double) data_image_raw[BLOCKSIZE * block + offset] / 0xff)

/* See svd.c for explanation. */
void svd(double **A, double *S2, int n);

int
main(void)
{
	/* Allocate double the needed space for future use by svd(). */
	double matrix[BLOCKSIZE * 2][BLOCKSIZE] = { 0 }, *mptr[BLOCKSIZE * 2], s2[BLOCKSIZE];

	/* Build the array of pointers to rows (needed for svd()). */
	for (size_t i = 0; i < BLOCKSIZE * 2; i += 1)
		mptr[i] = matrix[i];

	/* Calculate the correlation matrix for the input. */
	for (size_t i = 0; i < BLOCKSIZE; i += 1) {
		for (size_t j = 0; j < BLOCKSIZE; j += 1) {
			for (size_t block = 0; block < NUMBLOCKS; block += 1)
				matrix[i][j] += pixel(block, i) * pixel(block, j);
			matrix[i][j] /= NUMBLOCKS;
		}
	}

	/* Output the correlation matrix. */
	printf("\nCorrelation Matrix\n");
	for (size_t i = 0; i < BLOCKSIZE; i += 1) {
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			printf("%.6f ", matrix[i][j]);
		putchar('\n');
	}

	/* Perform singular value decomposition. */
	svd(mptr, s2, BLOCKSIZE);

	/* Output the transform matrix. */
	printf("\nTransform Matrix\n");
	for (size_t i = 0; i < BLOCKSIZE; i += 1) {
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			printf("%+.5f ", matrix[BLOCKSIZE + i][j]);
		putchar('\n');
	}

	return 0;
}
