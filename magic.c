#include <stdio.h>
#include <stdlib.h>

#include "magic.h"

#define MFLAGS (\
		MAGIC_SYMLINK | \
		MAGIC_MIME_TYPE | \
		MAGIC_MIME_ENCODING | \
		MAGIC_ERROR | \
		MAGIC_NO_CHECK_ASCII | \
		MAGIC_NO_CHECK_COMPRESS | \
		MAGIC_NO_CHECK_ELF | \
		MAGIC_NO_CHECK_TAR | \
		MAGIC_NO_CHECK_TOKENS)

magic_t mflags;

static void magic_init() __attribute__((constructor));
static void magic_init()
{
	mflags = magic_open(MFLAGS);
	if (mflags == NULL || magic_load(mflags, NULL) != 0) { /* load default database */
		fprintf(stderr, "could not init libmagic, exiting\n");
		exit(1);
	}
}
