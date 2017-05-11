/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include <imgcomp/imgcomp.h>

#define addbits(bitstring, numbits, tail)                                                          \
	do {                                                                                           \
		bitstring <<= numbits;                                                                     \
		bitstring |= tail & ((1U << numbits) - 1);                                                 \
	} while (0)
#define array_size(a) (sizeof(a) / sizeof((a)[0]))
#define min(a, b) (a < b ? a : b)

static uint32_t huffman_ac[11][16] = {
	{ 0x0000a, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x00073 },
	{ 0x0000c, 0x0001e, 0x0001b, 0x00013, 0x0003a, 0x0002c, 0x0007e, 0x00071, 0x0005a, 0x00049,
	  0x0004a, 0x000ee, 0x000e4, 0x000b6, 0x00097, 0x001de },
	{ 0x00008, 0x0003e, 0x0007f, 0x00070, 0x000ed, 0x00091, 0x001df, 0x001ab, 0x00121, 0x003b2,
	  0x003b1, 0x003b0, 0x002dc, 0x005bf, 0x006a7, 0x006a5 },
	{ 0x00017, 0x0006b, 0x00096, 0x001aa, 0x002de, 0x006a6, 0x005be, 0x00ecd, 0x00b77, 0x00d49,
	  0x01204, 0x03b31, 0x01202, 0x05bae, 0x01a90, 0x0240b },
	{ 0x00034, 0x001a8, 0x00767, 0x00903, 0x03b33, 0x01a91, 0x03b30, 0x07665, 0x0b749, 0x05bb5,
	  0x09007, 0x16e89, 0x16e82, 0x16ead, 0x16edb, 0x0b747 },
	{ 0x000e5, 0x00482, 0x01203, 0x16eb0, 0x09003, 0x16eaa, 0x09000, 0x16ea6, 0x16ea4, 0x16ea5,
	  0x16e8c, 0x16e8d, 0x16ece, 0x16ecf, 0x16eb4, 0x16eb6 },
	{ 0x00483, 0x16eb7, 0x16ec4, 0x16ec5, 0x0900e, 0x0900f, 0x16e9e, 0x16ea0, 0x16ea1, 0x16e9a,
	  0x16e9b, 0x16e9c, 0x16e9d, 0x16e98, 0x16e99, 0x16ec0 },
	{ 0x16ec1, 0x16ec2, 0x16ec3, 0x16e95, 0x16e96, 0x16e97, 0x16e90, 0x12056, 0x12057, 0x12054,
	  0x12055, 0x12052, 0x12053, 0x09028, 0x16e94, 0x1d993 },
	{ 0x16ebf, 0x16ebc, 0x16ebd, 0x16ec7, 0x16e87, 0x16e80, 0x16e81, 0x16ecc, 0x16ecd, 0x16e85,
	  0x16eca, 0x16ecb, 0x16ec8, 0x16ec9, 0x16ede, 0x09005 },
	{ 0x16edc, 0x16edd, 0x16ed8, 0x16ed9, 0x09006, 0x16e8b, 0x16ed1, 0x16ed2, 0x16ed3, 0x16e83,
	  0x16eac, 0x16eda, 0x16e88, 0x16ed0, 0x16e8a, 0x09008 },
	{ 0x09009, 0x16eb2, 0x09004, 0x16eb3, 0x0900a, 0x0900b, 0x16e84, 0x16eb1, 0x09002, 0x16e86,
	  0x16ec6, 0x16eab, 0x16ebe, 0x1d992, 0x1d990, 0x16ea7 }
};

static uint32_t huffman_dc[12] = { 0x001b, 0x000e, 0x0004, 0x0005, 0x000f, 0x000c,
	                               0x0035, 0x0069, 0x01a0, 0x01a1, 0x01a2, 0x01a3 };

static int flushbits(int fd, uint8_t *state);
static int read_ac(int fd, uint32_t *state, uint8_t *acpixels, int32_t *ac);
static int read_dc(int fd, uint32_t *state, int32_t *dc);
static int readbits(int fd, uint32_t *state, uint16_t *out, uint8_t numbits);
static int write_ac(int fd, uint8_t *state, uint8_t zeroes, int32_t value);
static int write_dc(int fd, uint8_t *state, int32_t value);
static int write_eob(int fd, uint8_t *state);
static int write_zrl(int fd, uint8_t *state);
static int writebits(int fd, uint8_t *state, uint32_t bitstring);

static int
flushbits(int fd, uint8_t *state)
{
	int err;
	uint8_t cursize;

	if (!state)
		return -EFAULT;

	cursize = log2u32(*state);

	if (cursize) {
		/* Shift *state left to put the data in the msb's. */
		*state <<= CHAR_BIT - cursize;
		if ((err = writeall(fd, state, 1)) < 0)
			goto out;
		/* The sentinel bit must be present for the next function call's log2u32. */
		*state = 1;
	}
	err = 0;

out:
	return err;
}

int
imagefile_read_huffman(int fd, size_t depth, struct image **img)
{
	int err;
	size_t block = 0;
	uint32_t state = 0;
	struct image *newimg;

	if (!img)
		return -EFAULT;
	if (!depth || depth > IMAGE_MAXDEPTH)
		return -EINVAL;

	if ((err = image_alloc(&newimg, TEST_SIZE, TEST_SIZE, depth)) < 0)
		goto out;

	while (block < newimg->height * newimg->width / BLOCKPIXELS) {
		int32_t ac, dc;
		uint8_t acpixels = 0;

		if ((err = read_dc(fd, &state, &dc)) < 0) {
			/* It is not an error to run out of data at the end of a block. */
			if (err == -EBADF)
				err = 0;
			break;
		}
		newimg->data[BLOCKPIXELS * block] = dc;
		while (acpixels < BLOCKPIXELS - 1) {
			/* Use goto to break out of two loops. */
			if ((err = read_ac(fd, &state, &acpixels, &ac)) < 0)
				goto done;
			newimg->data[BLOCKPIXELS * block + acpixels] = ac;
		}

		block += 1;
	}
done:
	if (err < 0) {
		image_free(newimg);
		goto out;
	}
	*img = newimg;
	err = 0;

out:
	return err;
}

int
imagefile_write_huffman(int fd, struct image *img)
{
	int err;
	uint8_t state = 0;

	if (!img)
		return -EFAULT;

	/* Iterate over each symbol (same as number of pixels, though no longer in pixel order). */
	for (size_t block = 0; block < img->height * img->width / (BLOCKPIXELS); block += 1) {
		uint8_t zeroes = 0;

		if ((err = write_dc(fd, &state, img->data[BLOCKPIXELS * block])) < 0)
			goto out;
		for (size_t i = 1; i < BLOCKPIXELS; i += 1) {
			if (!img->data[BLOCKPIXELS * block + i]) {
				if (i == BLOCKPIXELS - 1) {
					if ((err = write_eob(fd, &state)) < 0)
						goto out;
				} else {
					zeroes += 1;
				}
			} else {
				while (zeroes >= 16) {
					if ((err = write_zrl(fd, &state)) < 0)
						goto out;
					zeroes -= 16;
				}
				if ((err = write_ac(fd, &state, zeroes, img->data[BLOCKPIXELS * block + i])) < 0)
					goto out;
				zeroes = 0;
			}
		}
	}
	if ((err = flushbits(fd, &state)) < 0)
		goto out;
	err = 0;

out:
	return err;
}

static int
read_ac(int fd, uint32_t *state, uint8_t *acpixels, int32_t *ac)
{
	int err;
	uint8_t size, zeroes;
	uint16_t tmp;
	/* Start with the sentinel bit, just like the one prepended to each array entry. */
	uint32_t huff = 1;

	if (!state)
		return -EFAULT;

	/* Scan the Huffman table for a match. */
	while (1) {
		/* Read a single bit and append it to our candidate prefix. */
		if ((err = readbits(fd, state, &tmp, 1)) < 0)
			goto out;
		huff <<= 1;
		huff |= tmp;

		/* Try matching everything in the table to find the index. */
		for (size_t i = 0; i < array_size(huffman_ac); i += 1) {
			for (size_t j = 0; j < array_size(huffman_ac[i]); j += 1) {
				if (huffman_ac[i][j] == huff) {
					size = i;
					zeroes = j;
					goto found;
				}
			}
		}

		/* Bail if we read too much. */
		if (huff > 0x100000)
			goto out;
	}

found:
	/* Handle the run length (and the current pixel [the one with the value]), including EOB. */
	if (!size && !zeroes)
		*acpixels = 63;
	else
		*acpixels += zeroes + 1;

	/* Read the variable-length integer. */
	if (size) {
		if ((err = readbits(fd, state, &tmp, size)) < 0)
			goto out;
		*ac = tmp;
	} else {
		*ac = 0;
	}

	/* Apply translation for negative values. */
	if (*ac < 1 << (size - 1))
		*ac -= (1 << size) - 1;
	err = 0;

out:
	return err;
}

static int
read_dc(int fd, uint32_t *state, int32_t *dc)
{
	int err;
	uint8_t size;
	uint16_t tmp;
	/* Start with the sentinel bit, just like the one prepended to each array entry. */
	uint32_t huff = 1;

	if (!state)
		return -EFAULT;

	/* Scan the Huffman table for a match. */
	while (1) {
		/* Read a single bit and append it to our candidate prefix. */
		if ((err = readbits(fd, state, &tmp, 1)) < 0)
			goto out;
		huff <<= 1;
		huff |= tmp;

		/* Try matching everything in the table to find the index. */
		for (size_t i = 0; i < array_size(huffman_dc); i += 1) {
			if (huffman_dc[i] == huff) {
				size = i;
				goto found;
			}
		}

		/* Bail if we read too much. */
		if (huff > 0x10000)
			goto out;
	}

found:
	if (size) {
		/* Read the variable-length integer. */
		if ((err = readbits(fd, state, &tmp, size)) < 0)
			goto out;
		*dc = tmp;
	} else {
		*dc = 0;
	}

	/* Apply translation for negative values. */
	if (*dc < 1 << (size - 1))
		*dc -= (1 << size) - 1;
	err = 0;

out:
	return err;
}

static int
readbits(int fd, uint32_t *state, uint16_t *out, uint8_t numbits)
{
	int err;
	uint8_t cursize;

	if (!state)
		return -EFAULT;
	if (numbits > (sizeof(*out) * CHAR_BIT))
		return -EINVAL;

	cursize = log2u32(*state);

	/* Read more bits from the file if necessary. */
	if (numbits > cursize) {
		size_t morebytes = (numbits - cursize + CHAR_BIT - 1) / CHAR_BIT;

		/* Record the additional space we have filled. */
		cursize += morebytes * CHAR_BIT;
		/* Shift the existing data out of the way */
		*state <<= morebytes * CHAR_BIT;
		/* Living dangerously... assumes little-endian. */
		if ((err = readall(fd, state, morebytes)) < 0)
			goto out;
	}

	/* Copy the most significant numbits bits to the output variable. */
	*out = *state >> (cursize - numbits);
	*out &= (1U << numbits) - 1;
	cursize -= numbits;
	/* Create a new sentinel. */
	*state |= 1 << cursize;
	/* Erase everything to the left of the sentinel. */
	*state &= (1U << (cursize + 1)) - 1;
	err = 0;

out:
	return err;
}

static int
write_ac(int fd, uint8_t *state, uint8_t zeroes, int32_t value)
{
	uint8_t size;
	uint32_t bitstring = 1;

	if (!state)
		return -EFAULT;

	size = log2i32(value);
	if (value < 0)
		value += (1 << size) - 1;
	addbits(bitstring, log2u32(huffman_ac[size][zeroes]), huffman_ac[size][zeroes]);
	if (size)
		addbits(bitstring, size, value);

	return writebits(fd, state, bitstring);
}

static int
write_dc(int fd, uint8_t *state, int32_t value)
{
	uint8_t size;
	uint32_t bitstring = 1;

	if (!state)
		return -EFAULT;

	size = log2i32(value);
	if (value < 0)
		value += (1 << size) - 1;
	addbits(bitstring, log2u32(huffman_dc[size]), huffman_dc[size]);
	if (size)
		addbits(bitstring, size, value);

	return writebits(fd, state, bitstring);
}

static int
write_eob(int fd, uint8_t *state)
{
	return write_ac(fd, state, 0, 0);
}

static int
write_zrl(int fd, uint8_t *state)
{
	return write_ac(fd, state, 15, 0);
}

static int
writebits(int fd, uint8_t *state, uint32_t bitstring)
{
	int err;
	uint8_t cursize, newsize;

	if (!state)
		return -EFAULT;

	cursize = log2u32(*state);
	newsize = log2u32(bitstring);

	/* Loop until there are no unprocessed bits left. */
	while (newsize) {
		/* Copy the minimum of the free space in *state, and the number of bits we have to copy. */
		uint8_t copysize = min(CHAR_BIT - cursize, newsize);

		/* Shift *state left to make room to copy n bits into the lsb's. */
		*state <<= copysize;
		/* Shift the bitstring right to extract the top n bits, and copy them into *state. */
		*state |= (bitstring >> (newsize - copysize)) & ((1U << copysize) - 1);
		/* The *state variable now contains n more bits... */
		cursize += copysize;
		/* And the bitstring contains that many fewer. */
		newsize -= copysize;

		/* If *state is full, write it out. */
		if (cursize == CHAR_BIT) {
			if ((err = writeall(fd, state, 1)) < 0)
				goto out;
			cursize = 0;
			/* The sentinel bit must be present for the next function call's log2u32. */
			*state = 1;
		}
	}
	err = 0;

out:
	return err;
}
