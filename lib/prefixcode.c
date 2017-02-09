/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <internal/heap.h>
#include <internal/utils.h>
#include <squish/prefixcode.h>

#define TABLE_SIZE (ALPHABET_SIZE * sizeof(uint32_t))

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

	/* Allocate a scratch buffer for building the Huffman tree. This needs to be twice the alphabet
	 * size to hold all of the internal nodes. */
	if (!(nodes = calloc(ALPHABET_SIZE * 2, sizeof(struct huffman_node)))) {
		status = -errno;
		goto out;
	}
	/* Copy the distribution to the nodes that will form the heap. Each symbol in the alphabet must
	 * have a nonzero probability, or combining nodes would result in an infinite loop of 0 + 0 = 0.
	 * While this does skew the distribution, the difference will be negligible for all but the
	 * smallest of data sets. */
	for (size_t i = 0; i < ALPHABET_SIZE; i += 1) {
		nodes[i].count = dist->counts[i] + 1;
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

out:
	if (heap)
		heap_free(heap);
	if (status < 0 && newcode)
		free(nodes);
	if (nodes)
		free(nodes);
	return status;
}

long
huffman_compress(void *buffer, size_t size, void *output, size_t *outsize)
{
	long status = 0;
	size_t osize;
	unsigned char *obuffer = 0;
	struct distribution *dist = 0;
	struct prefixcode *code = 0;

	if (!buffer || !size || !output || !outsize)
		return -EINVAL;

	if (!(dist = distribution_new()))
		goto out;
	if ((status = buffer_distribution(dist, buffer, size)) < 0)
		goto out;
	if ((status = huffman_code(dist, &code)) < 0)
		goto out;
	/* Make room for storing the table with the compressed data. Size is in bits. */
	osize = TABLE_SIZE * 8;
	/* Calculate the size of the compressed data. */
	for (size_t i = 0; i < ALPHABET_SIZE; i += 1)
		osize += dist->counts[i] * log2u32(code->words[i]);
	/* Convert output size to bytes. Round up. */
	osize = (osize + CHAR_BIT - 1) / CHAR_BIT;

	/* Allocate the output buffer. */
	if (!(obuffer = calloc(1, osize)))
		goto out;
	/* Copy the Huffman table. That the type size matches the reserved size is not an accident. */
	memcpy(obuffer, code->words, TABLE_SIZE);
	/* Encode the data. */
	if ((status = prefix_encode(code, buffer, size, obuffer + TABLE_SIZE, osize - TABLE_SIZE)) < 0)
		goto out;

	/* Send back our result. */
	*(void **) output = obuffer;
	*outsize = osize;

out:
	if (code)
		free(code);
	if (dist)
		distribution_free(dist);
	if (status < 0 && obuffer)
		free(obuffer);
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

long
prefix_encode(struct prefixcode *code, void *buffer, size_t size, void *output, size_t outsize)
{
	size_t byte = 0;
	unsigned char *inbuf = buffer, *outbuf = output;
	unsigned int bit = 0;

	if (!code || !buffer || !size || !output || !outsize)
		return -EINVAL;

	for (size_t i = 0; i < size; i += 1) {
		uint32_t word = code->words[inbuf[i]];
		unsigned int wordlen = log2u32(word);

		/* Prepare the word by shifting it to the far left (throwing away the leading 1). */
		word <<= 32 - wordlen;

		/* First, fill any remaining bits in the current byte. */
		if (bit) {
			/* Find the number of available bits left in this byte. */
			unsigned int todo = CHAR_BIT - bit;
			/* This should leave the first 'todo' bits of the word in the lowest 'todo' bits. */
			unsigned char tmp = word >> (32 - todo);
			/* Set those bits in the current output byte. */
			outbuf[byte] |= tmp;
			if (wordlen >= todo) {
				/* This filled the byte. Move on to the next byte. */
				bit = 0;
				byte += 1;
				/* There may be bits left to write. Shift out the already-written ones. */
				word <<= todo;
				wordlen -= todo;
			} else {
				/* This wrote the whole word, and this byte still has available bits in it. */
				bit += wordlen;
				wordlen = 0;
			}
		}

		/* Fill out more bytes as needed, and as space allows. */
		while (wordlen) {
			if (byte >= outsize)
				return -ENOSPC;
			/* Set this output byte to the high byte of the code word. */
			outbuf[byte] = word >> 24;
			if (wordlen >= 8) {
				/* This was a full byte. bit is still 0. */
				byte += 1;
				wordlen -= 8;
			} else {
				/* This was the last byte, and it was a partial byte. */
				bit += wordlen;
				wordlen = 0;
			}
		}
	}

	return 0;
}
