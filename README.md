## About
Squish is a suite of data compression and analysis tools. It consists of a library and several
command-line utilities.

## Building
Run `make` to compile everything and run the test suite. Compiled binaries will be placed in
build/bin, and the library will be in build/lib. Alternatively, run `make bin` or `make lib` to make
just the binaries or the library respectively. You can use config.mk to customize compiler options
or perform an out-of-tree build.

Squish strives to be portable across all platforms conforming to POSIX.1-2008 or newer.

## File format
A file compressed in the squish format contains a file-level header followed by one or more blocks
of (most likely compressed) data. Each block of data begins with a block header describing its type,
size, and checksum. Integers in both headers are stored in network byte order. Blocks are a multiple
of eight bytes long, padded with zeroes as necessary.

### File-level header
| Size    | Contents
| ------- | ----------------------------------------------------------------------------------------
| 6 bytes | the ASCII/UTF-8 string "squiSH"
| 1 byte  | the major version number as an ASCII/UTF-8 digit, e.g. "1"
| 1 byte  | the minor version number as an ASCII/UTF-8 digit, e.g. "0"
| 4 bytes | reserved (must be zero)
| 4 bytes | the number of blocks in the file

### Block-level header
| Size    | Contents
| ------- | ----------------------------------------------------------------------------------------
| 8 bytes | block type
| 4 bytes | compressed block size (bits)
| 4 bytes | uncompressed data size (bits)

### Block type field
| Size    | Contents
| ------- | ----------------------------------------------------------------------------------------
| 8 bits  | stream identifier (e.g. data 1, data 2, data n, metadata)
| 8 bits  | checksum method
| 8 bits  | authentication method
| 8 bits  | encryption method
| 8 bits  | outer compression method
| 8 bits  | inner compression method
| 8 bits  | preprocessing/quantization method
| 8 bits  | reserved (must be zero)
