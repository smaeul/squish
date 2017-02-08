/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <errno.h>
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
	int data[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 }, element;
	struct heap *heap;

	/* Success case #2. */
	heap = heap_new(data, sizeof(int), SIZE, maxheap);
	assert(heap);
	for (int i = SIZE - 1; i >= 0; i -= 1) {
		assert(heap_extract(heap, &element) == 0);
		assert(element == i);
	}
	assert(heap_extract(heap, &element) == -ENOENT);
	assert(element == 0);
	assert(heap_free(heap) == 0);

	heap = heap_new(data, sizeof(int), SIZE, minheap);
	assert(heap);
	for (int i = 0; i < SIZE; i += 1) {
		assert(heap_extract(heap, &element) == 0);
		assert(element == i);
	}
	assert(heap_extract(heap, &element) == -ENOENT);
	assert(element == SIZE - 1);
	assert(heap_free(heap) == 0);

	return 0;
}
