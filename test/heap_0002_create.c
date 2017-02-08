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

static int
minheap(void *a, void *b)
{
	return *(int *) b - *(int *) a;
}

int
main(void)
{
	int data[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	struct heap *heap;

	/* Success case #2. */
	heap = heap_new(data, sizeof(int), SIZE, maxheap);
	assert(heap);
	assert(heap_getcapacity(heap) == SIZE);
	assert(heap_getsize(heap) == SIZE);
	assert(heap_free(heap) == 0);

	heap = heap_new(data, sizeof(int), SIZE, minheap);
	assert(heap);
	assert(heap_getcapacity(heap) == SIZE);
	assert(heap_getsize(heap) == SIZE);
	assert(heap_free(heap) == 0);

	return 0;
}
