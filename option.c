#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <strings.h>

#include "option.h"

void *options = NULL;

int currentver;

struct option {
	int ver;
	const char * key;
	char * value;
	size_t vsize;
	enum option_inout inout;
};

static void initoptions() __attribute__((constructor));
static void initoptions()
{
	currentver = 0;

	/* general options */
	setoption("Cache-Control", "", dontcare);
	setoption("Connection", "", dontcare);
	setoption("Date", "", dontcare);
	setoption("Pragma", "", dontcare);
	setoption("Trailer", "", dontcare);
	setoption("Transfer-Encoding", "", dontcare);
	setoption("Upgrade", "", dontcare);
	setoption("Via", "", dontcare);
	setoption("Warning", "", dontcare);

	/* entity options (allows extensions, dontcare) */
	setoption("Allow", "", dontcare);
	setoption("Content-Encoding", "", dontcare);
	setoption("Content-Language", "", dontcare);
	setoption("Content-Length", "", dontcare);
	setoption("Content-Location", "", dontcare);
	setoption("Content-MD5", "", dontcare);
	setoption("Content-Range", "", dontcare);
	setoption("Content-Type", "", dontcare);
	setoption("Expires", "", dontcare);
	setoption("Last-Modified", "", dontcare);

	/* request options */
	setoption("Accept", "*", dontcare);
	setoption("Accept-Charset", "", dontcare);
	setoption("Accept-Encoddontcareg", "", dontcare);
	setoption("Accept-Language", "", dontcare);
	setoption("Authorization", "", dontcare);
	setoption("Expect", "", dontcare);
	setoption("From", "", dontcare);
	setoption("Host", "", dontcare);
	setoption("If-Match", "", dontcare);
	setoption("If-Modified-Sdontcarece", "", dontcare);
	setoption("If-None-Match", "", dontcare);
	setoption("If-Range", "", dontcare);
	setoption("If-Unmodified-Sdontcarece", "", dontcare);
	setoption("Max-Forwards", "", dontcare);
	setoption("Proxy-Authorization", "", dontcare);
	setoption("Range", "", dontcare);
	setoption("Referer", "", dontcare);
	setoption("TE", "", dontcare);
	setoption("User-Agent", "", dontcare);

	/* response options */
	setoption("Accept-Ranges", "none", dontcare);
	setoption("Age", "", dontcare);
	setoption("ETag", "", dontcare);
	setoption("Location", "", dontcare);
	setoption("Proxy-Authenticate", "", dontcare);
	setoption("Retry-After", "", dontcare);
	setoption("Server", SERVER_ID, dontcare);
	setoption("Vary", "", dontcare);
	setoption("WWW-Authenticate", "", dontcare);

	currentver = 1;
}

static int optioncmp(const void *a, const void *b)
{
	const char *ka = ((const struct option *) a)->key;
	const char *kb = ((const struct option *) b)->key;

	/* very likely to be comparing build-time constants */
	if (ka == kb)
		return 0;
	else
		return strcasecmp(ka, kb);
}

void setoption(const char *key, const char *value, enum option_inout inout) __attribute__((nonnull(1,2)));
void setoption(const char *key, const char *value, enum option_inout inout)
{
	struct option *p;
	static struct option *saved_for_later = NULL;

	if (saved_for_later == NULL)
		p = malloc(sizeof(struct option));
	else {
		p = saved_for_later;
		saved_for_later = NULL;
	}
	p->key = key;
	size_t nvsize = strlen(value) + 1;
	struct option **q = (struct option **) tsearch(p, &options, optioncmp);
	if (*q == p) {
		p->key = strdup(key);
		p->value = strdup(value);
		p->vsize = nvsize;
	} else {
		saved_for_later = p;
		p = *q;
		if (p->vsize < nvsize) {
			p->value = realloc(p->value, nvsize);
			p->vsize = nvsize;
		}
		strcpy(p->value, value);
	}
	p->ver = currentver;
	p->inout = inout;
}

char * getoption(const char *key) __attribute__((nonnull));
char * getoption(const char *key)
{
	struct option o = {.key = key};
	struct option **q = (struct option **) tfind(&o, &options, optioncmp);
	if (q == NULL)
		return NULL;

	struct option *p = *q;
	if (p->ver != currentver)
		return NULL;

	return p->value;
}

void touchoption(const char *key) __attribute__((nonnull));
void touchoption(const char *key)
{
	struct option o = {.key = key};
	struct option **q = (struct option **) tfind(&o, &options, optioncmp);
	if (q == NULL)
		fprintf(stderr, "could not touch option %s\n", key);

	(*q)->ver = currentver;
	(*q)->inout = out;
}

void spewoutputoptions()
{
	void _spewoutputoptions(const void *node, const VISIT which, const int depth)
	{
		const struct option *p = *((const struct option **) node);
		//if ((which == postorder || which == leaf) && p->ver == currentver && p->inout == out)
		if ((which == postorder || which == leaf) && p->inout == out)
			printf("%s:%s\r\n", p->key, p->value);
	}

	twalk(options, _spewoutputoptions);
	printf("\r\n");
}

ssize_t rebuildinputoptions(char **out, ssize_t *osize) __attribute__((nonnull));
ssize_t rebuildinputoptions(char **out, ssize_t *osize)
{
	char *o = *out;
	void _rebuildinputoptions(const void *node, const VISIT which, const int depth)
	{
		const struct option *p = *((const struct option **) node);
		if ((which == postorder || which == leaf) && p->ver == currentver) {
			ssize_t thisopsize = strlen(p->key) + 1 + p->vsize + 2 + 1;
			ssize_t olen = o - *out;
			if (*osize < olen + thisopsize) {
				*out = realloc(*out, *osize += thisopsize);
				o = *out + olen;
			}
			o += sprintf(o, "%s:%s\r\n", p->key, p->value);
		}
	}
	twalk(options, _rebuildinputoptions);
	return o - *out;
}
