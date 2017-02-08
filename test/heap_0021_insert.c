/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <internal/heap.h>

static int
maxheap(void *a, void *b)
{
	return *(int *) a - *(int *) b;
}

int
main(void)
{
	int data = 42, element;
	struct heap *heap;

	/* Success case #1. */
	heap = heap_new(&data, sizeof(int), 1, maxheap);
	assert(heap);
	assert(heap_getsize(heap) == 1);
	assert(heap_extract(heap, &element) == 0);
	assert(heap_getsize(heap) == 0);
	assert(element == 42);
	assert(heap_insert(heap, &element) == 0);
	assert(heap_getsize(heap) == 1);
	element = 20;
	assert(heap_extract(heap, &element) == 0);
	assert(heap_getsize(heap) == 0);
	assert(element == 42);
	assert(heap_free(heap) == 0);

	return 0;
}
