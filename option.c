#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>

#include "option.h"

void *options = NULL;
int optionver;

static void initoptions() __attribute__((constructor));
static void initoptions()
{
	optionver = 0;

	/* general options */
	setoptionof("Cache-Control", "", general);
	setoptionof("Connection", "", general);
	setoptionof("Date", "", general);
	setoptionof("Pragma", "", general);
	setoptionof("Trailer", "", general);
	setoptionof("Transfer-Encoding", "", general);
	setoptionof("Upgrade", "", general);
	setoptionof("Via", "", general);
	setoptionof("Warning", "", general);

	/* entity options (allows extensions) */
	setoptionof("Allow", "", entity);
	setoptionof("Content-Encoding", "", entity);
	setoptionof("Content-Language", "", entity);
	setoptionof("Content-Length", "", entity);
	setoptionof("Content-Location", "", entity);
	setoptionof("Content-MD5", "", entity);
	setoptionof("Content-Range", "", entity);
	setoptionof("Content-Type", "", entity);
	setoptionof("Expires", "", entity);
	setoptionof("Last-Modified", "", entity);

	/* request options */
	setoptionof("Accept", "", request);
	setoptionof("Accept-Charset", "", request);
	setoptionof("Accept-Encoding", "", request);
	setoptionof("Accept-Language", "", request);
	setoptionof("Authorization", "", request);
	setoptionof("Expect", "", request);
	setoptionof("From", "", request);
	setoptionof("Host", "", request);
	setoptionof("If-Match", "", request);
	setoptionof("If-Modified-Since", "", request);
	setoptionof("If-None-Match", "", request);
	setoptionof("If-Range", "", request);
	setoptionof("If-Unmodified-Since", "", request);
	setoptionof("Max-Forwards", "", request);
	setoptionof("Proxy-Authorization", "", request);
	setoptionof("Range", "", request);
	setoptionof("Referer", "", request);
	setoptionof("TE", "", request);
	setoptionof("User-Agent", "", request);

	/* response options */
	setoptionof("Accept-Ranges", "", response);
	setoptionof("Age", "", response);
	setoptionof("ETag", "", response);
	setoptionof("Location", "", response);
	setoptionof("Proxy-Authenticate", "", response);
	setoptionof("Retry-After", "", response);
	setoptionof("Server", SERVER_ID, response);
	setoptionof("Vary", "", response);
	setoptionof("WWW-Authenticate", "", response);

	optionver = 1;
}

static int optioncmp(const void *a, const void *b)
{
	return strcasecmp(((const struct option *) a)->key, ((const struct option *) b)->key);
}

void setoptionof(const char *key, const char *value, enum option_type type)
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
		p->ver = optionver;
		p->key = strdup(key);
		p->value = strdup(value);
		p->vsize = nvsize;
	} else {
		saved_for_later = p;
		p = *q;
		if (type != unknown && p->type != type) {
			fprintf(stderr, "dangerously changing option type (%s):\n\tfrom: %d\n\tto: %d\n", p->key, p->type, type);
			p->type = type;
		}
		if (p->ver == optionver)
			fprintf(stderr, "overwriting option value (%s):\n\tfrom: %s\n\tto: %s\n", p->key, p->value, value);
		else
			p->ver = optionver;
		if (p->vsize < nvsize) {
			p->value = realloc(p->value, nvsize);
			p->vsize = nvsize;
		}
		strcpy(p->value, value);
	}
}

void setoption(const char *key, const char *value)
{
	setoptionof(key, value, unknown);
}

char * getoption(const char *key)
{
	struct option o = {.key = key};
	struct option **q = (struct option **) tfind(&o, &options, optioncmp);
	if (q == NULL)
		return NULL;

	struct option *p = *q;
	if (p->ver != optionver)
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
	if (p->ver != optionver || p->type != type)
		return NULL;

	return p->value;
}
