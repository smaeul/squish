/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define LAYERS 1
#define RATIO 2
#define TESTSIZE 256

#define nterms(f) (2 * (f)->offset + 1)
#define px(x, y) ((x) * (r->isize) + (y))

struct filter {
	unsigned int offset;  /**< Maximum offset from the center (length is 2 * offset + 1). */
	unsigned int divisor; /**< Divisor for after summation. */
	int16_t *terms;       /**< Actual terms used for summation. */
};

struct region {
	size_t bsize; /**< Side length of the block we are working with at the moment. */
	size_t isize; /**< Side length of the entire image. */
	size_t x;     /**< Row offset (from the top) of the block we are working with. */
	size_t y;     /**< Column offset (from the left) of the block we are working with. */
};

static bool analyze(int16_t *input, int16_t *output, struct region *r);
static bool downsample(int16_t *input, int16_t *output, struct region *r, bool righthalf);
static bool filter(int16_t *input, int16_t *output, struct region *r, struct filter *f);
static bool filter_row(int16_t *input, int16_t *output, struct region *r, struct filter *f,
                       size_t row);
static bool process(char *file, unsigned int layers);
static bool saveimage(int16_t *image, struct region *r, char *file, char *suffix);
static bool selftest(size_t size);
static bool synthesize(int16_t *input, int16_t *output, struct region *r);
static bool transpose(int16_t *image, struct region *r);
static bool upsample(int16_t *input, int16_t *output, struct region *r, bool righthalf);
static bool writeall(int fd, uint8_t *buffer, size_t size);

static struct filter h0 = { 2, 8, (int16_t[]){ -1, 2, 6, 2, -1 } };
static struct filter h1 = { 2, 2, (int16_t[]){ -1, 2, -1, 0, 0 } };
static struct filter g0 = { 1, 2, (int16_t[]){ 1, 2, 1 } };
static struct filter g1 = { 3, 8, (int16_t[]){ 0, 0, -1, -2, 6, -2, -1 } };

static bool
analyze(int16_t *input, int16_t *output, struct region *r)
{
	int16_t *temp;

	if (!input || !output || !r)
		return false;

	/* Allocate a temporary buffer for filtering results. */
	if (!(temp = calloc((r->isize * r->isize), sizeof(int16_t))))
		return false;
	/* Apply h0 to the input and store the results in temp. */
	if (!filter(input, temp, r, &h0))
		return false;
	/* Downsample the filter results into the left half of the output. */
	if (!downsample(temp, output, r, false))
		return false;
	/* Apply h1 to the input and store the results in temp. */
	if (!filter(input, temp, r, &h1))
		return false;
	/* Downsample the filter results into the right half of the output. */
	if (!downsample(temp, output, r, true))
		return false;

	free(temp);
	return true;
}

static bool
downsample(int16_t *input, int16_t *output, struct region *r, bool righthalf)
{
	size_t startcol;

	if (!input || !output || !r)
		return false;

	startcol = r->y + (righthalf ? r->bsize / RATIO : 0);
	for (size_t i = 0; i < r->bsize; i += 1)
		for (size_t j = 0; j < r->bsize; j += RATIO)
			output[px(r->x + i, startcol + j / RATIO)] = input[px(r->x + i, r->y + j)];

	return true;
}

static bool
filter(int16_t *input, int16_t *output, struct region *r, struct filter *f)
{
	if (!input || !output || !f || !r)
		return false;

	for (size_t i = 0; i < r->bsize; i += 1)
		if (!filter_row(input, output, r, f, i))
			return false;

	return true;
}

static bool
filter_row(int16_t *input, int16_t *output, struct region *r, struct filter *f, size_t row)
{
	int16_t *temp;

	if (!input || !output || !f || !r)
		return false;

	/* Allocate a temporary buffer, including space for symmetric extension. */
	if (!(temp = calloc((r->bsize + 2 * f->offset), sizeof(int16_t))))
		return false;
	/* Left side symmetric extension, left to right in input, so right to left in temp. */
	for (size_t i = 0; i < f->offset; i += 1)
		temp[f->offset - i - 1] = input[px(r->x + row, r->y + i + 1)];
	/* Copy the input as is. */
	for (size_t i = 0; i < r->bsize; i += 1)
		temp[f->offset + i] = input[px(r->x + row, r->y + i)];
	/* Right side symmetric extension, right to left in input, so left to right in temp. */
	for (size_t i = 0; i < f->offset; i += 1)
		temp[r->bsize + f->offset + i] = input[px(r->x + row, r->y + r->bsize - i - 2)];
	/* Perform filtering on each pixel. */
	for (size_t i = 0; i < r->bsize; i += 1) {
		int16_t sum = 0;
		for (size_t t = 0; t < nterms(f); t += 1)
			sum += f->terms[t] * temp[i + t];
		sum /= f->divisor;
		output[px(r->x + row, r->y + i)] = sum;
	}

	free(temp);
	return true;
}

static bool
process(char *file, unsigned int layers)
{
	double mse = 0;
	int fd;
	int16_t *alpha, *beta;
	off_t size;
	uint8_t *original;
	struct region r;

	if (!file)
		return false;

	if ((fd = open(file, O_RDONLY)) < 0)
		return false;
	if ((size = lseek(fd, 0, SEEK_END)) == -1)
		return false;
	if ((original = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return false;
	/* The file can be closed as soon as the memory mapping is made. */
	close(fd);

	if (!(alpha = calloc(size, sizeof(int16_t))))
		return false;
	if (!(beta = calloc(size, sizeof(int16_t))))
		return false;
	/* Copy the original image so we can ping-pong between two buffers. */
	for (ssize_t i = 0; i < size; i += 1)
		alpha[i] = original[i] - 128;
	/* Assume the image is sqare. Then its side length is the square root of its size. */
	r.bsize = r.isize = sqrt(size);
	r.x = r.y = 0;
	printf("    File is %zu bytes long, so this must be a %zux%zu pixel image\n", size, r.isize,
	       r.isize);

	/* Decompose. */
	for (size_t i = 1; i <= layers; i += 1) {
		/* Set the region of interest. */
		r.bsize = r.isize / (1 << (i - 1));

		printf("    Analyzing layer %zu\n        Horizontally...\n", i);
		/* Set beta = horizontally analyzed. */
		if (!analyze(alpha, beta, &r))
			return false;
		/* Set beta = horizontally analyzed + transposed. */
		if (!transpose(beta, &r))
			return false;
		printf("        Vertically...\n");
		/* Set alpha = horizontally analyzed + transposed + vertically analyzed. */
		if (!analyze(beta, alpha, &r))
			return false;
		/* Set alpha = horizontally analyzed + vertically analyzed (undo transposition). */
		if (!transpose(alpha, &r))
			return false;
	}

	/* Save the full decomposed image. */
	r.bsize = r.isize;
	saveimage(alpha, &r, file, "decomposed");

	/* Reconstruct. Analyzed image is in alpha. */
	for (size_t i = layers; i >= 1; i -= 1) {
		/* Set the region of interest. */
		r.bsize = r.isize / (1 << (i - 1));

		printf("    Synthesizing layer %zu\n        Vertically...\n", i);
		/* Set alpha = transposed. */
		if (!transpose(alpha, &r))
			return false;
		/* Set beta = transposed + vertically synthesized. */
		if (!synthesize(alpha, beta, &r))
			return false;
		printf("        Horizontally...\n");
		/* Set beta = vertically synthesized (undo transposition). */
		if (!transpose(beta, &r))
			return false;
		/* Set alpha = vertically synthesized + horizontally synthesized. */
		if (!synthesize(beta, alpha, &r))
			return false;
	}

	/* Save the full reconstructed image. */
	r.bsize = r.isize;
	saveimage(alpha, &r, file, "reconstructed");

	/* Calcualate the mean squared error from the original. */
	for (off_t i = 0; i < size; i += 1) {
		mse += pow(alpha[i] + 128 - original[i], 2);
	}
	mse /= size;
	printf("    Mean error squared is %.2f brightness levels\n", mse);

	free(alpha);
	free(beta);
	munmap(original, size);
	return true;
}

static bool
saveimage(int16_t *image, struct region *r, char *file, char *suffix)
{
	char fname[PATH_MAX];
	int fd;
	uint8_t *buffer;

	if (!image || !r || !file || !suffix)
		return false;

	if (snprintf(fname, PATH_MAX, "%s.%s", file, suffix) >= PATH_MAX)
		return false;
	if (!(buffer = calloc((r->isize * r->isize), sizeof(uint8_t))))
		return false;
	if ((fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
		return false;
	/* Convert back to one byte per pixel. */
	for (size_t i = 0; i < r->isize * r->isize; i += 1)
		buffer[i] = image[i] + 128;
	if (!writeall(fd, buffer, r->isize * r->isize * sizeof(uint8_t)))
		return false;
	printf("        Saved image to '%s'\n", fname);

	close(fd);
	return true;
}

static bool
selftest(size_t size)
{
	int16_t *buffer;
	struct region r = { size, size, 0, 0 };

	/* Allocate "image". */
	if (!(buffer = calloc(size * size, sizeof(int16_t))))
		return false;
	/* Generate input data */
	for (size_t i = 0; i < size; i += 1)
		for (size_t j = 0; j < size; j += 1)
			buffer[i * size + j] = i + j;

	saveimage(buffer, &r, "testimage", "raw");
	process("testimage.raw", 1);

	free(buffer);
	return true;
}

static bool
synthesize(int16_t *input, int16_t *output, struct region *r)
{
	int16_t *temp, *temp2;

	if (!input || !output || !r)
		return false;

	/* Allocate a temporary buffer for upsampling results. */
	if (!(temp = calloc((r->isize * r->isize), sizeof(int16_t))))
		return false;
	/* Allocate a temporary buffer for summation of the filter results. */
	if (!(temp2 = calloc((r->isize * r->isize), sizeof(int16_t))))
		return false;
	/* Upsample the left half of the input into temp. */
	if (!upsample(input, temp, r, false))
		return false;
	/* Apply g0 to the temp buffer and store the results in output. */
	if (!filter(temp, output, r, &g0))
		return false;
	/* Upsample the right half of the input into temp. */
	if (!upsample(input, temp, r, true))
		return false;
	/* Apply g1 to the temp buffer and store the results in temp2. */
	if (!filter(temp, temp2, r, &g1))
		return false;
	/* Sum the temp2 buffer into the output. */
	for (size_t i = 0; i < r->bsize; i += 1)
		for (size_t j = 0; j < r->bsize; j += 1)
			output[px(r->x + i, r->y + j)] += temp2[px(r->x + i, r->y + j)];

	free(temp);
	free(temp2);
	return true;
}

static bool
transpose(int16_t *image, struct region *r)
{
	if (!image || !r)
		return false;

	/* Iterate through the upper-right triangle, swapping with the lower-left triangle. */
	for (size_t i = 0; i < r->bsize; i += 1) {
		for (size_t j = i + 1; j < r->bsize; j += 1) {
			int16_t temp = image[px(r->x + i, r->y + j)];
			image[px(r->x + i, r->y + j)] = image[px(r->x + j, r->y + i)];
			image[px(r->x + j, r->y + i)] = temp;
		}
	}

	return true;
}

static bool
upsample(int16_t *input, int16_t *output, struct region *r, bool righthalf)
{
	size_t startcol;

	if (!input || !output || !r)
		return false;

	startcol = r->y + (righthalf ? r->bsize / RATIO : 0);
	for (size_t i = 0; i < r->bsize; i += 1) {
		for (size_t j = 0; j < r->bsize / RATIO; j += 1) {
			output[px(r->x + i, r->y + j * RATIO)] = input[px(r->x + i, startcol + j)];
			output[px(r->x + i, r->y + j * RATIO + 1)] = 0;
		}
	}

	return true;
}

static bool
writeall(int fd, uint8_t *buffer, size_t size)
{
	size_t written = 0;
	ssize_t current;

	if (fd < 0 || !buffer || !size)
		return false;

	while (written < size) {
		if ((current = write(fd, buffer + written, size - written)) < 0) {
			/* Always retry if interrupted. Never retry after other errors. */
			if (errno == EINTR)
				continue;
			else
				break;
		}
		/* Add this round to the total if it actually represents a number of bytes. */
		written += current;
	}

	return written == size;
}

int
main(int argc, char *argv[])
{
	int error = 0;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s file...\n", argv[0]);
		return 1;
	}

	printf("Performing self-test on filtering functions\n");
	if (!selftest(TESTSIZE))
		return 1;

	for (int i = 1; i < argc; i += 1) {
		printf("Processing file '%s'\n", argv[i]);
		if (!process(argv[i], LAYERS))
			error = 1;
	}

	return error;
}
