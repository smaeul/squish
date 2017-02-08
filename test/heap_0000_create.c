/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <string.h>

#include <internal/heap.h>

#define SIZE 10

static int
maxheap(void *a, void *b)
{
	return *(int *) a - *(int *) b;
}

int
main(void)
{
	int data[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

	/* Failure cases: null pointers. */
	assert(!(heap_new(0, sizeof(int), SIZE, maxheap)));
	assert(!(heap_new(data, 0, SIZE, maxheap)));
	assert(!(heap_new(data, sizeof(int), 0, maxheap)));
	assert(!(heap_new(data, sizeof(int), SIZE, 0)));

	return 0;
}
