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

int
main(void)
{
	int data[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 }, element;
	struct heap *heap;

	/* Success case #2 (maxheap). */
	heap = heap_new(data, sizeof(int), SIZE, maxheap);
	assert(heap);
	/* Clear the heap. */
	for (int i = SIZE - 1; i >= 0; i -= 1) {
		assert(heap_extract(heap, &element) == 0);
		assert(element == i);
	}
	assert(heap_getsize(heap) == 0);

	/* This is a max-heap. Fill it in order (no swaps). */
	for (int i = 0; i < SIZE; i += 1) {
		assert(heap_insert(heap, &data[i]) == 0);
	}
	assert(heap_getsize(heap) == heap_getcapacity(heap));
	assert(heap_insert(heap, &element) == -ENOSPC);
	/* Verify the heap property. */
	for (int i = SIZE - 1; i >= 0; i -= 1) {
		assert(heap_extract(heap, &element) == 0);
		assert(element == i);
	}
	assert(heap_getsize(heap) == 0);

	/* Now fill it in reverse order (all swaps). */
	for (int i = SIZE - 1; i >= 0; i -= 1) {
		assert(heap_insert(heap, &data[i]) == 0);
	}
	assert(heap_getsize(heap) == heap_getcapacity(heap));
	assert(heap_insert(heap, &element) == -ENOSPC);
	/* Verify the heap property. */
	for (int i = SIZE - 1; i >= 0; i -= 1) {
		assert(heap_extract(heap, &element) == 0);
		assert(element == i);
	}
	assert(heap_getsize(heap) == 0);

	assert(heap_free(heap) == 0);

	return 0;
}
