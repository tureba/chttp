#ifndef OPTION_H
#define OPTION_H

#include <unistd.h>

#include "config.h"

enum option_type {
		general,
		entity,

		request,
		response,

		unknown
};

enum option_inout { in, out, dontcare };

extern int currentver;

void setoptionof(const char *key, const char *value, enum option_type type, enum option_inout inout);
#define setinoption(key,value) setoptionof(key, value, unknown, in)
#define setoutoption(key,value) setoptionof(key, value, unknown, out)

char * getoption(const char *key);
char * getoptionof(const char *key, enum option_type type);

void touchoption(const char *key);

void spewoutputoptions();
ssize_t rebuildinputoptions(char **out, ssize_t *osize);

#endif /* OPTION_H */
