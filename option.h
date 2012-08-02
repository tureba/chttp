#ifndef OPTION_H
#define OPTION_H

#include "config.h"

enum option_type {
		general,
		entity,

		request,
		response,

		unknown
};

struct option {
	enum option_type type;
	int ver;
	const char * key;
	char * value;
	size_t vsize;
};

extern void *options;
extern int optionver;

void setoptionof(const char *key, const char *value, enum option_type type);
void setoption(const char *key, const char *value);

char * getoption(const char *key);
char * getoptionof(const char *key, enum option_type type);

#endif /* OPTION_H */
