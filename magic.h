#ifndef MAGIC_H
#define MAGIC_H

#include <magic.h>

extern magic_t mflags;

#define getfilemime(f) magic_file(mflags, f)
#define getbuffermime(b,l) magic_buffer(mflags, b, l)

#endif /* MAGIC_H */
