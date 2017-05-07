/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_ERROR_H
#define IMGCOMP_ERROR_H

#define ERR_BASE 4096
#define ERR_NONE 0
#define ERR_NULL -EFAULT   /**< A null pointer was passed to a function. */
#define ERR_INVAL -EINVAL  /**< The arguments to the function are invalid. */
#define ERR_NOTSUP -ENOSYS /**< The image format is not supported. */
#define ERR_ALLOC -ENOMEM  /**< Unable to allocate memory. */
#define ERR_NODATA -EBADF  /**< No(t enough) data could be read from the file. */

#endif /* IMGCOMP_ERROR_H */
