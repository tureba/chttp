#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "option.h"

char method[METHOD_SIZE_INT];
int httpver[2];
char addr[ADDR_SIZE_INT];

int getreq(void)
{
	char key[50], value[2048];

	/* Read header in the form: */
	/* METHOD ADDRESS HTTP/x.y\r\n */
	if (scanf(" %" METHOD_SIZE_TEXT "s", method) != 1)
		return 1;

	if (scanf(" %" ADDR_SIZE_TEXT "s", addr) != 1)
		return 0;

	if (scanf(" %*[Hh] %*[Tt] %*[Pp] / %u . %u\r", &httpver[0], &httpver[1]) != 2)
		return 0;

	/* Read the options in the form: */
	/* KEY : VALUE\r\n */
	while (scanf(" %49[^: \t] : %2047[^\r]\r", key, value) == 2)
		setoption(key, value);

	return 0;
}

int main (int argc, char **argv, char **envp)
{
	while (!getreq())
		optionver++;

	return 0;
}
