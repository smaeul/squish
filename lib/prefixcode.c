/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include <internal/heap.h>
#include <internal/utils.h>
#include <squish/prefixcode.h>

struct huffman_node {
	struct huffman_node *left;  /**< The left child of this node (0 for leaf nodes). */
	struct huffman_node *right; /**< The right child of this node (0 for leaf nodes). */
	size_t count;               /**< The sum of the counts of all leaves below this node */
	unsigned char value;        /**< The value this leaf node encodes (if this is a leaf). */
};

struct prefixcode {
	uint32_t words[ALPHABET_SIZE]; /**< The code word for each symbol in the alphabet. */
};

/**
 * Predicate function for comparison of Huffman heap nodes (based on count).
 * @param a A huffman_node structure
 * @param b A huffman_node structure
 * @return An integer value with sign such that the higher-count node is placed higher in the heap
 */
static int huffman_heap(void *a, void *b);

/**
 * Recursively store the path of each node in the tree in the prefixcode structure.
 * @param node The tree of huffman_node structures to traverse
 * @param path The path to the current node
 * @param code The prefixcode structure to update with each leaf's path
 * @return 0 if the operation was successful, or a negative value representing an error
 */
static long huffman_traverse(struct huffman_node *node, unsigned int path, struct prefixcode *code);

long
huffman_code(struct distribution *dist, struct prefixcode **code)
{
	long status = 0;
	unsigned int offset = 0;
	struct heap *heap = 0;
	struct huffman_node left, *nodes = 0, parent, right, *root;
	struct prefixcode *newcode = 0;

	if (!dist || !code)
		return -EINVAL;

	/* Ensure each symbol in the alphabet has a nonzero probability, or combining nodes would result
	 * in an infinite loop of 0+0 = 0. */
	/* TODO: make a copy to avoid modifying the original distribution. */
	if ((status = distribution_adjust(dist)) < 0)
		goto out;
	/* Allocate a scratch buffer for building the Huffman tree. This needs to be twice the alphabet
	 * size to hold all of the internal nodes. */
	if (!(nodes = calloc(ALPHABET_SIZE * 2, sizeof(struct huffman_node)))) {
		status = -errno;
		goto out;
	}
	for (size_t i = 0; i < ALPHABET_SIZE; i += 1) {
		nodes[i].count = dist->counts[i];
		nodes[i].value = i;
	}
	/* Create a min-heap from the adjusted distribution in the scratch buffer. */
	if (!(heap = heap_new(nodes, sizeof(struct huffman_node), ALPHABET_SIZE, huffman_heap))) {
		/* XXX: this is why heap_new should return a status. */
		status = -ENOMEM;
		goto out;
	}

	/* Until the heap is empty: take two out, create a parent (sum) node, and stick it back in. */
	while ((status = heap_extract(heap, &left)) >= 0) {
		if ((status = heap_extract(heap, &right)) < 0) {
			/* Abort on any real error. */
			if (status != -ENOENT)
				goto out;

			/* Otherwise, there was only one node left in the heap, and it becomes the root. */
			nodes[offset] = left;
			root = &nodes[offset];

			/* Break to avoid calling a heap_extract() that is guaranteed to fail. */
			status = 0;
			break;
		}

		/* Copy the extracted nodes to the tree buffer. */
		nodes[offset] = left;
		nodes[offset + 1] = right;

		/* Create a parent for these two nodes and insert it into the heap. */
		parent = (struct huffman_node){
			.count = left.count + right.count, .left = &nodes[offset], .right = &nodes[offset + 1],
		};
		if ((status = heap_insert(heap, &parent)) < 0)
			goto out;

		/* Update the offset into the tree buffer. */
		offset += 2;
	}
	/* There should be no error state here, since status is reset the only time heap_extract() is
	 * expected to fail. */
	if (status < 0)
		goto out;

	if (!(newcode = calloc(1, sizeof(struct prefixcode)))) {
		status = -errno;
		goto out;
	}
	/* Generate the final prefix code representation. Each code word is prepended with a one to
	 * preserve leading zeroes. */
	if ((status = huffman_traverse(root, 1, newcode)) < 0)
		goto out;

	/* Send the created codebook back to the caller. */
	*code = newcode;
	/* Avoid freeing the codebook. */
	newcode = 0;

out:
	if (heap)
		heap_free(heap);
	if (newcode)
		free(nodes);
	if (nodes)
		free(nodes);
	return status;
}

static int
huffman_heap(void *a, void *b)
{
	return ((struct huffman_node *) b)->count - ((struct huffman_node *) a)->count;
}

static long
huffman_traverse(struct huffman_node *node, unsigned int path, struct prefixcode *code)
{
	long status = 0;

	if (!node || !code)
		return -EINVAL;

	if (!node->left && !node->right) {
		/* This is a leaf node. */
		code->words[node->value] = path;
	} else {
		if (node->left) {
			/* The left node gets a zero in its encoding. */
			status = huffman_traverse(node->left, path << 1, code);
		}
		if (node->right) {
			/* The right node gets a one in its encoding. */
			status = huffman_traverse(node->right, path << 1 | 1, code);
		}
	}

	return status;
}
