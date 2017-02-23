/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON 0.0001  /* Minimum improvement per iteration */
#define SAMPLES 10000   /* Number of random data points to generate */
#define SEED 0xdeadbeef /* Pseudorandom number generator seed */
#define STDDEV 10       /* Random distribution sqrt(variance) */
#define TRIALS 100      /* Maximum number of partitions */

struct partition {
	double decision;       /**< The maximum value that will be placed in this partition */
	double reconstruction; /**< The value this partition will be decoded to */
};

struct result {
	double distortion; /**< The total squared error caused by quantization */
	double entropy;    /**< The number of bits needed to encode each quantized symbol */
};

/* Callback function for qsort(3) to operate on doubles */
static int doublesort(const void *, const void *);
/* Fill an array with random data following a Gaussian distribution */
static bool fillrandom(double *, size_t);
/* Generate a pair of random values belonging to a Gaussian distribution */
static void getgrandom(double *, double *);
/* Quantize an array of doubles and return quality metrics */
static bool quantize(double *, size_t, unsigned, struct result *);

static int
doublesort(const void *x, const void *y)
{
	return *(double *) x - *(double *) y > 0 ? 1 : -1;
}

static bool
fillrandom(double *data, size_t length)
{
	double a, b;

	if (!data || !length)
		return false;

	/* Fill the array two elements at a time. */
	for (size_t i = 0; i < length / 2 * 2; i += 2) {
		getgrandom(&a, &b);
		data[i] = a;
		data[i + 1] = b;
	}
	/* Fill the last element of odd-length arrays. */
	if (length % 2) {
		getgrandom(&a, &b);
		data[length - 1] = a;
	}

	return true;
}

static void
getgrandom(double *a, double *b)
{
	double s, u, v;

	/* Use the polar Box–Muller transform. */
	do {
		/* random(3) returns values in [0,2^31). Scale them to [-1,1). */
		u = (double) (random() - (1L << 30)) / (1L << 30);
		v = (double) (random() - (1L << 30)) / (1L << 30);
		s = u * u + v * v;
	} while (s >= 1.0 || s == 0.0);
	s = sqrt(-2.0 * log(s) / s);
	*a = u * s * STDDEV;
	*b = v * s * STDDEV;
}

static bool
quantize(double *data, size_t length, unsigned partitions, struct result *output)
{
	double distortion, previous;
	struct partition *partdata = 0;

	if (!data || !length || !partitions || !output)
		return false;

	/* Allocate space to hold decision/reconstruction levels. */
	if (!(partdata = calloc(partitions, sizeof(struct partition))))
		return false;

	/* Step 1: choose initial decision levels and initialize distortion. */
	for (unsigned i = 0; i < partitions; i += 1)
		partdata[i].decision = data[0] + (data[length - 1] - data[0]) * (i + 1) / partitions;
	output->distortion = INFINITY;

	do {
		/* Step 2: compute reconstruction levels. */
		for (size_t i = 0, x = 0; i < partitions; i += 1) {
			/* Record the beginning of the set. */
			size_t first = x;
			/* Reset the reconstruction level for summation. */
			partdata[i].reconstruction = 0;
			/* Loop through the data points until the value is past the decision level. */
			while (x < length && data[x] <= partdata[i].decision) {
				partdata[i].reconstruction += data[x];
				x += 1;
			}
			/* Divide by the length of the set to find the centroid. */
			partdata[i].reconstruction /= x - first;
		}

		/* Step 3: update decision levels. */
		for (unsigned i = 0; i < partitions - 1; i += 1) {
			/* Each decision level should be halfway between neighboring reconstruction levels. */
			partdata[i].decision =
			    (partdata[i].reconstruction + partdata[i + 1].reconstruction) / 2;
		}

		/* Step 4: calculate quantizer distortion. */
		distortion = 0;
		for (size_t i = 0, x = 0; i < partitions; i += 1) {
			/* Loop through the data points until the value is past the decision level. */
			while (x < length && data[x] <= partdata[i].decision) {
				double distance = data[x] - partdata[i].reconstruction;
				/* Add each point's contribution to the distortion. */
				distortion += distance * distance / length;
				x += 1;
			}
		}

		/* Step 5: compare current distortion to the previous iteration. */
		previous = output->distortion;
		output->distortion = distortion;
	} while (!((previous - distortion) / distortion < EPSILON));

	/* Now that the partition is optimized, calculate its entropy. */
	output->entropy = 0;
	for (size_t i = 0, x = 0; i < partitions; i += 1) {
		double probability;
		/* Record the beginning of the partition. */
		size_t first = x;
		/* Loop through the data points until the value is past the decision level. */
		while (x < length && data[x] <= partdata[i].decision) {
			x += 1;
		}
		/* log2(0) is undefined. An empty partition should never happen, but handle it anyway. */
		if (x == first)
			continue;
		/* Calculate the probability of a data point being in this partition. */
		probability = (double) (x - first) / length;
		/* Add this partition's contribution to the entropy. */
		output->entropy -= probability * log2(probability);
	}

	if (partdata)
		free(partdata);
	return true;
}

int
main(void)
{
	double samples[SAMPLES] = {};
	struct result results[TRIALS] = {};

	/* Generate a set of random samples following a Gaussian distribution. */
	srandom(SEED);
	if (!fillrandom(samples, SAMPLES))
		goto err;

	/* Sort the samples for simpler processing. */
	qsort(samples, SAMPLES, sizeof(double), doublesort);

	/* Perform quantization of the data for varying numbers of partitions. */
	for (unsigned m = 1; m <= TRIALS; m += 1) {
		if (!quantize(samples, SAMPLES, m, &results[m - 1]))
			goto err;
	}

	/* Report the data. */
	for (unsigned m = 0; m < TRIALS; m += 1)
		printf("%.8f %.8f\n", results[TRIALS - m - 1].distortion, results[TRIALS - m - 1].entropy);

	return 0;

err:
	return 1;
}
