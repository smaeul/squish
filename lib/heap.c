/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <internal/heap.h>
#include <internal/utils.h>

#define elem_addr(x) (&heap->data[heap->elsize * (x)])

struct heap {
	size_t capacity; /**< The total amount of space in the heap (allocated memory). */
	size_t elsize;   /**< The size of each element in bytes. */
	size_t size;     /**< The number of elements currently in the heap. */
	int (*predicate)(void *, void *); /**< The comparision function */
	unsigned char data[];             /**< The actual elements in the heap. */
};

/**
 * Put an element at the given index in the correct place in the heap. This implements the
 * min-/max-heapify operation.
 * @param heap The heap to operate on
 * @param index The index of an element in the heap that might be in the wrong position
 * @return 0 if the operation was successful, or a negative value representing an error
 */
static long heap_fix(struct heap *heap, size_t index);

long
heap_extract(struct heap *heap, void *element)
{
	long status;

	if (!heap || !element)
		return -EINVAL;
	if (!heap->size)
		return -ENOENT;

	heap->size -= 1;
	/* If the heap was previously of size 1 (so now 0), there is nothing to swap, and heap_fix()
	 * would fail. */
	if (heap->size) {
		/* Swap the first element with the former last element. */
		memswap(elem_addr(0), elem_addr(heap->size), heap->elsize);
		/* Fix the heap, starting from the new first element. */
		if ((status = heap_fix(heap, 0)) < 0)
			return status;
	}
	/* Copy the old first element to the user-supplied buffer. */
	memcpy(element, elem_addr(heap->size), heap->elsize);

	return 0;
}

static long
heap_fix(struct heap *heap, size_t index)
{
	if (!heap)
		return -EINVAL;
	if (index >= heap->size)
		return -EDOM;

	/* Loop until we reach an element without children (and therefore nothing left to swap). */
	while (index < heap->size / 2) {
		size_t left = index * 2 + 1, right = index * 2 + 2, upper;

		/* Because of the while condition above, the current element must have a left child. */
		upper = heap->predicate(elem_addr(index), elem_addr(left)) >= 0 ? index : left;
		/* The current element may or may not have a right child. */
		if (right < heap->size && heap->predicate(elem_addr(upper), elem_addr(right)) < 0)
			upper = right;
		/* If this element is in the right place, there is nothing more to do. */
		if (upper == index)
			break;
		/* Replace this element with the child that should be in its place. */
		memswap(elem_addr(index), elem_addr(upper), heap->elsize);
		/* Continue processing with the child the original value was swapped into. */
		index = upper;
	}

	return 0;
}

long
heap_free(struct heap *heap)
{
	if (!heap)
		return -EINVAL;

	free(heap);

	return 0;
}

size_t
heap_getcapacity(struct heap *heap)
{
	if (!heap)
		return 0;

	return heap->capacity;
}

size_t
heap_getsize(struct heap *heap)
{
	if (!heap)
		return 0;

	return heap->size;
}

long
heap_insert(struct heap *heap, void *element)
{
	size_t index;

	if (!heap || !element)
		return -EINVAL;
	if (heap->size == heap->capacity)
		return -ENOSPC;

	/* Copy the new element into the next slot after the end of the heap, and extend it. */
	memcpy(elem_addr(heap->size), element, heap->elsize);
	heap->size += 1;

	/* Bubble the element up the heap as needed (like a simplified reverse heap_fix()). */
	index = heap->size - 1;
	while (index > 0 && heap->predicate(elem_addr(index), elem_addr(index / 2)) > 0) {
		memswap(elem_addr(index), elem_addr(index / 2), heap->elsize);
		index /= 2;
	}

	return 0;
}

struct heap *
heap_new(void *elements, size_t elsize, size_t nelem, int (*predicate)(void *, void *))
{
	struct heap *heap = 0;

	/* XXX: NULL return prevents explaining failure! */
	if (!elements || !elsize || !nelem || !predicate)
		return 0;
	if (nelem >= SIZE_MAX / elsize) {
		/* Muliplication would overflow. */
		return 0;
	}

	/* Allocate and initialize a heap structure. */
	if (!(heap = calloc(1, sizeof(struct heap) + nelem * elsize)))
		return 0;
	heap->capacity = nelem;
	heap->elsize = elsize;
	heap->size = nelem;
	heap->predicate = predicate;
	/* Copy the provided data into the heap memory. */
	memcpy(heap->data, elements, nelem * elsize);

	/* Build a heap from the unordered data. Only nodes with children need to be considered.
	 * NOTE: this loop counter must be signed to avoid an infinite loop. Conveniently, we start at
	 * half the maximum index, so range is never an issue. */
	for (ssize_t i = nelem / 2 - 1; i >= 0; i -= 1)
		if (heap_fix(heap, i) < 0)
			goto error;

	return heap;

error:
	if (heap)
		free(heap);
	return 0;
}
