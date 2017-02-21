/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef SQUISH_ATTRIBUTES_H
#define SQUISH_ATTRIBUTES_H

/*#if defined(__GNUC__)
#define alloc __attribute__((malloc))
#define leaf __attribute__((leaf))
#define must_check __attribute__((warn_unused_result))
#define pure __attribute__((pure))
#define unused __attribute__((unused))
#define used __attribute__((used))
#else
#define alloc
#define leaf
#define must_check
#define pure
#define unused
#define used
#endif*/

#if defined(__GNUC__)
#define likely(x) (__builtin_expect(!!(x), 1))
#define unlikely(x) (__builtin_expect((x), 0))
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#endif /* SQUISH_ATTRIBUTES_H */
