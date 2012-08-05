#ifndef OPTION_H
#define OPTION_H

#include <unistd.h>

#include "config.h"

enum option_inout { in, out, dontcare };

extern int currentver;

void setoption(const char *key, const char *value, enum option_inout inout);
#define setinoption(key,value) setoption(key, value, in)
#define setoutoption(key,value) setoption(key, value, out)

char * getoption(const char *key);

void touchoption(const char *key);

void spewoutputoptions();
ssize_t rebuildinputoptions(char **out, ssize_t *osize);

#endif /* OPTION_H */
