/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <internal/heap.h>

#define SIZE 1

static int
maxheap(void *a, void *b)
{
	return *(int *) a - *(int *) b;
}

int
main(void)
{
	int data = 42, element = 20;
	struct heap *heap;

	/* Failure cases: null pointers. */
	heap = heap_new(&data, sizeof(int), SIZE, maxheap);
	assert(heap);
	assert(heap_extract(0, &element) == -EINVAL);
	assert(element == 20);
	assert(heap_extract(heap, 0) == -EINVAL);
	assert(heap_free(heap) == 0);

	return 0;
}
