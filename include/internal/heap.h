/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef INTERNAL_HEAP_H
#define INTERNAL_HEAP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque structure representing a heap
 */
struct heap;

/**
 * Extract the first element from the heap.
 * @param heap The heap to operate on
 * @param element Memory where the extracted element will be stored
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long heap_extract(struct heap *heap, void *element);

/**
 * Free an existing heap.
 * @param heap A pointer to the heap
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long heap_free(struct heap *heap);

/**
 * Get the capacity of a heap.
 * @param heap A pointer to the heap
 * @return The capacity of the heap, or 0 if the heap is not valid.
 */
size_t heap_getcapacity(struct heap *heap);

/**
 * Get the current size of a heap.
 * @param heap A pointer to the heap
 * @return The size of the heap, or 0 if the heap is not valid.
 */
size_t heap_getsize(struct heap *heap);

/**
 * Insert a new element into a heap. This heap implementation cannot grow beyond its initial size.
 * @param heap The heap to operate on
 * @param element A pointer to the new element
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long heap_insert(struct heap *heap, void *element);

/**
 * Create a heap from an array of data. The heap will have a capacity as large as this array.
 * The data in the array is copied into the heap; the array is not referenced after this function
 * returns. The return value of the predicate should be as follows: positive if first element goes
 * on top (i.e. toward the root of the heap), negative if second goes on top, or zero if the two
 * elements could go in either order (i.e. are the same).
 * @param elements The data to put into the heap
 * @param elsize The size of each element in bytes
 * @param nelem The number of elements in the source array
 * @param predicate A function called to compare elements in the heap.
 * @return A pointer to the heap if the operation was successful, or 0 if there was an error
 */
struct heap *heap_new(void *elements, size_t elsize, size_t nelem,
                      int (*predicate)(void *, void *));

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_HEAP_H */
