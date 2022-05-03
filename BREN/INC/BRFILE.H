/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brfile.h 1.7 1995/02/22 21:36:59 sam Exp $
 * $Locker:  $
 *
 * Brender's interface to file IO
 */

#ifndef _BRFILE_H_
#define _BRFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef int BR_CALLBACK br_mode_test_cbfn(br_uint_8 *magics,br_size_t n_magics);

/*
 * Interface to filesystem
 */
typedef br_uint_32 BR_CALLBACK brfile_attributes_cbfn(void);
typedef void * BR_CALLBACK brfile_open_read_cbfn(char *name, br_size_t n_magics,
	br_mode_test_cbfn *mode_test,
	int *mode_result);
typedef void * BR_CALLBACK brfile_open_write_cbfn(char *name, int text);
typedef void BR_CALLBACK brfile_close_cbfn(void * f);
typedef int BR_CALLBACK brfile_eof_cbfn(void * f);
typedef int	BR_CALLBACK brfile_getchr_cbfn(void * f);
typedef void BR_CALLBACK brfile_putchr_cbfn(int c, void * f);
typedef br_size_t BR_CALLBACK brfile_read_cbfn(void *buf, br_size_t size, unsigned int nelems, void * f);
typedef br_size_t BR_CALLBACK brfile_write_cbfn(void *buf, br_size_t size, unsigned int nelems, void * f);
typedef br_size_t BR_CALLBACK brfile_getline_cbfn(char *buf, br_size_t buf_len, void * f);
typedef void BR_CALLBACK brfile_putline_cbfn(char *buf, void * f);
typedef void BR_CALLBACK brfile_advance_cbfn(br_size_t count, void * f);


typedef struct br_filesystem {
	char *identifier;

	/*
	 * Inquire about attributes of file system
	 */
	brfile_attributes_cbfn *attributes;

	/*
	 * Locate and open a file, and then grab 'n_magics' bytes
	 * from the front of the file and pass them to a an 'identify'
	 * function that returns the mode of the file
	 *
	 * Returns file handle, or NULL if open failed
	 *
	 * Stores open mode through mode_result if not NULL
	 */
	brfile_open_read_cbfn *open_read;

	/*
	 * Open a file for writing, possibly with text translation
	 *
	 * Returns fil handle, or NULL if open failed
	 */
	brfile_open_write_cbfn *open_write;

	/*
	 * Close an opened file
	 */
	brfile_close_cbfn *close;

	/*
	 * Find out if at end of file
	 */
	brfile_eof_cbfn *eof;

	/*
	 * Read and write single characters
	 */
	brfile_getchr_cbfn *getchr;
	brfile_putchr_cbfn *putchr;

	/*
	 * Read and write blocks
	 */
	brfile_read_cbfn *read;
	brfile_write_cbfn *write;

	/*
	 * Read and write lines of text, excluding any line terminators
	 * Will only be used if  text translation is in use
	 */
	brfile_getline_cbfn *getline;
	brfile_putline_cbfn *putline;

	/*
	 * Advance <count> bytes through stream
	 *
	 * Will not be invoked if text translation is in use
	 */
	brfile_advance_cbfn *advance;

} br_filesystem;

/*
 * Bitmask returned by fs->attributes
 */
enum br_filesystem_attributes {
	BR_FS_ATTR_READABLE      = 0x0001,
	BR_FS_ATTR_WRITEABLE     = 0x0002,
	BR_FS_ATTR_HAS_TEXT      = 0x0004,
	BR_FS_ATTR_HAS_BINARY    = 0x0008,
	BR_FS_ATTR_HAS_ADVANCE   = 0x0010
};

/*
 * Possible values returner by open_read identify callback
 */
enum br_filesystem_identify {
	BR_FS_MODE_BINARY,
	BR_FS_MODE_TEXT,
	BR_FS_MODE_UNKNOWN
};

/*
 * Maximum number of magic bytes that can be requested on open_read
 */
#define BR_MAX_FILE_MAGICS 16

/*
 * Returned by filesys->getchr at end of file
 */
#define BR_EOF (-1)


#ifdef __cplusplus
};
#endif
#endif

