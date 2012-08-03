#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <strings.h>

#include "option.h"

void *options = NULL;

int currentver;

struct option {
	enum option_type type;
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
	setoptionof("Cache-Control", "", general, dontcare);
	setoptionof("Connection", "", general, dontcare);
	setoptionof("Date", "", general, dontcare);
	setoptionof("Pragma", "", general, dontcare);
	setoptionof("Trailer", "", general, dontcare);
	setoptionof("Transfer-Encoding", "", general, dontcare);
	setoptionof("Upgrade", "", general, dontcare);
	setoptionof("Via", "", general, dontcare);
	setoptionof("Warning", "", general, dontcare);

	/* entity options (allows extensions, dontcare) */
	setoptionof("Allow", "", entity, dontcare);
	setoptionof("Content-Encoding", "", entity, dontcare);
	setoptionof("Content-Language", "", entity, dontcare);
	setoptionof("Content-Length", "", entity, dontcare);
	setoptionof("Content-Location", "", entity, dontcare);
	setoptionof("Content-MD5", "", entity, dontcare);
	setoptionof("Content-Range", "", entity, dontcare);
	setoptionof("Content-Type", "", entity, dontcare);
	setoptionof("Expires", "", entity, dontcare);
	setoptionof("Last-Modified", "", entity, dontcare);

	/* request options */
	setoptionof("Accept", "", request, in);
	setoptionof("Accept-Charset", "", request, in);
	setoptionof("Accept-Encoding", "", request, in);
	setoptionof("Accept-Language", "", request, in);
	setoptionof("Authorization", "", request, in);
	setoptionof("Expect", "", request, in);
	setoptionof("From", "", request, in);
	setoptionof("Host", "", request, in);
	setoptionof("If-Match", "", request, in);
	setoptionof("If-Modified-Since", "", request, in);
	setoptionof("If-None-Match", "", request, in);
	setoptionof("If-Range", "", request, in);
	setoptionof("If-Unmodified-Since", "", request, in);
	setoptionof("Max-Forwards", "", request, in);
	setoptionof("Proxy-Authorization", "", request, in);
	setoptionof("Range", "", request, in);
	setoptionof("Referer", "", request, in);
	setoptionof("TE", "", request, in);
	setoptionof("User-Agent", "", request, in);

	/* response options */
	setoptionof("Accept-Ranges", "", response, out);
	setoptionof("Age", "", response, out);
	setoptionof("ETag", "", response, out);
	setoptionof("Location", "", response, out);
	setoptionof("Proxy-Authenticate", "", response, out);
	setoptionof("Retry-After", "", response, out);
	setoptionof("Server", SERVER_ID, response, out);
	setoptionof("Vary", "", response, out);
	setoptionof("WWW-Authenticate", "", response, out);

	currentver = 1;
}

static int optioncmp(const void *a, const void *b)
{
	return strcasecmp(((const struct option *) a)->key, ((const struct option *) b)->key);
}

void setoptionof(const char *key, const char *value, enum option_type type, enum option_inout inout)
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
		p->type = type;
		p->ver = currentver;
		p->key = strdup(key);
		p->value = strdup(value);
		p->vsize = nvsize;
		p->inout = inout;
	} else {
		saved_for_later = p;
		p = *q;
		if (type != unknown && p->type != type) {
			fprintf(stderr, "dangerously changing option type (%s):\n\tfrom: %d\n\tto: %d\n", p->key, p->type, type);
			p->type = type;
		}
		if (p->ver == currentver)
			fprintf(stderr, "overwriting option value (%s):\n\tfrom: %s\n\tto: %s\n", p->key, p->value, value);
		else
			p->ver = currentver;
		if (p->vsize < nvsize) {
			p->value = realloc(p->value, nvsize);
			p->vsize = nvsize;
		}
		p->inout = inout;
		strcpy(p->value, value);
	}
}

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

char * getoptionof(const char *key, enum option_type type)
{
	struct option o = {.key = key};
	struct option **q = (struct option **) tfind(&o, &options, optioncmp);
	if (q == NULL)
		return NULL;

	struct option *p = *q;
	if (p->ver != currentver || p->type != type)
		return NULL;

	return p->value;
}

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
		const struct option *p = (const struct option *) node;
		if ((which == postorder || which == leaf) && p->ver == currentver && p->type == response && p->inout == out)
			printf("%s:%s\r\n", p->key, p->value);
	}

	twalk(options, _spewoutputoptions);
	printf("\r\n");
}


ssize_t rebuildinputoptions(char **out, ssize_t *osize)
{
	char *o = *out;
	void _rebuildinputoptions(const void *node, const VISIT which, const int depth)
	{
		const struct option *p = (const struct option *) node;
		if ((which == postorder || which == leaf) && p->ver == currentver && p->inout == in) {
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
