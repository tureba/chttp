#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>

#include "config.h"
#include "statuscodes.h"
#include "option.h"
#include "magic.h"

char method[METHOD_SIZE + 1];
char addr[ADDR_SIZE + 1];
unsigned int httpver[2];

int main(void)
{
	char key[KEY_SIZE + 1], value[VALUE_SIZE + 1];
	char *content = NULL;
	ssize_t content_size = 0;
	int n1, n2, m1, m2;
	/* response chosen for this request */
	/* leave uninitialized so the compiler warns */
	int response;
	int terminate = 0;

	do { /* main loop */

		/* Read header in the form: */
		/* METHOD ADDRESS HTTP/x.y\r\n */
		switch (scanf(" %n%" INTTOSTR(METHOD_SIZE) "[^ \f\v\t\r\n]%n %n%" INTTOSTR(ADDR_SIZE)
					"[^ \f\v\t\r\n]%n %*[Hh]%*[Tt]%*[Pp] / %u . %u\r\n",
					&n1, method, &n2, &m1, addr, &m2, &httpver[0], &httpver[1])) {
			case -1: /* EOF */
				return 0;

			case 0: /* I don't know if this could happen, but let's retry */
				continue;

			case 1:
			case 2:
			case 3:
				if ((n2 - n1) >= METHOD_SIZE) {
					fprintf(stderr, TEXTTOSTR(METHOD_SIZE) " too short (" INTTOSTR(METHOD_SIZE) ") for method %s...\n", method);
					response = 400; /* Bad Request */
					terminate = 1; /* let's not try to recover from this */
				} else if ((m2 - m1) >= ADDR_SIZE) {
					fprintf(stderr, TEXTTOSTR(ADDR_SIZE) " too short (" INTTOSTR(ADDR_SIZE) ") for addr %s...\n", addr);
					response = 414; /* Request-URI Too Long */
					/* discard the rest of the line and retry */
					scanf(" %*[^\r]\r\n");
				} else {
					response = 400; /* Bad Request */
					/* discard the rest of the line and retry */
					scanf(" %*[^\r]\r\n");
				}
				goto respond;

			case 4:
			default:
				/* move on to reading the options */
				break;
		}

		/* Read the options in the form: */
		/* KEY : VALUE\r\n */
		do {
			switch (scanf(" %n%" INTTOSTR(KEY_SIZE) "[^: \f\v\t\r\n]%n : %n%" INTTOSTR(VALUE_SIZE)
						"[^ \f\v\t\r\n]%n \r\n", &n1, key, &n2, &m1, value, &m2)) {
				case 0:
					if (scanf(" \r\n") == 0)
						/* end of options */
						goto break2;
					else /* EOF possibly? */
						/* passthrough */;

				case 1: /* key or value too long or : ou \r not found */
					if ((n2 - n1) >= KEY_SIZE || (m2 - m1) >= VALUE_SIZE) {
						fprintf(stderr, "key (size %d of %d) or value (size %d of %d) was too long\n", n2 - n1, KEY_SIZE, m2 - m1, VALUE_SIZE);
						response = 413; /* Request Entity Too Long */
						terminate = 1;
						goto respond;
					} else /* possibly not key:value\r\n format */
						/* passthrough to Bad Request */;

				case -1: /* EOF */
					/* expected \r\n, but nevermind */
					terminate = 1;
					goto break2;

				case 2:
				default:
					setinoption(key, value);
			}
		} while (1);
break2:

		if (getoption("Content-Length") == NULL) {
			response = 411; /* Length Required */
			goto respond;
		}

		/* utility function for OPTIONS, GET, HEAD... */
		void discard_input_content()
		{
			ssize_t content_len = atoi(getoption("Content-Length"));
			while (content_len > 0) {
				/* maybe use some other general-purpose buffer? */
				ssize_t n = read(0, value, (content_len <= VALUE_SIZE) ? content_len : VALUE_SIZE);
				if (n == -1) /* EOF, no more content, in spite of Content-Length */
					return;
				content_len -= n;
			}
		}

		if (!strcasecmp(method, "OPTIONS")) {
			response = 200; /* OK */
			discard_input_content();
			setoutoption("Accept","*/*");
			setoutoption("Content-Length", "0"); /* force this */
		} else if (!strcasecmp(method, "TRACE")) {
			response = 200; /* OK */

			ssize_t header_len = snprintf(value, VALUE_SIZE + 1, "%s %s HTTP/%u.%u\r\n", method, addr, httpver[0], httpver[1]);
			ssize_t content_len = atoi(getoption("Content-Length"));
			ssize_t options_len = rebuildinputoptions(&content, &content_size);
			ssize_t total_len = header_len + options_len + 2 + content_len + 1;

			if (content_size < total_len)
				content = realloc(content, content_size = total_len);

			/* make space in the beginning for the header */
			memmove(content + header_len, content, options_len);
			/* copy the header to its position */
			memcpy(content, value, header_len);
			/* put \r\n between the options and the original content */
			content[header_len + options_len] = '\r';
			content[header_len + options_len + 1] = '\n';
			/* bring the original content into the outgoing message */
			read(0, content + header_len + options_len + 2, content_len); /* don't really care about what this returns */
			/* put the response's Content-Length in place */
			snprintf(key, KEY_SIZE + 1, "%d", total_len - 1); /* the last \0 is not going to be streamed */
			setoutoption("Content-Length", key);
		} else { /* most valid methods fall into this ATM */
			fprintf(stderr, "received request for method %s\n", method);
			response = 405; /* Method Not Allowed */
			setoutoption("Allow", "OPTIONS");
			setoutoption("Content-Length", "0"); /* force this */
		}

respond:
		touchoption("Server"); /* this should always be sent */
		if (getoption("Content-Length") == NULL)
			setoutoption("Content-Length", "0");
		if (!response) {
			fprintf(stderr, "execution path not predicted\n");
			response = 500; /* Internal Server Error */
			setoutoption("Content-length", "0");
		}

		/* status line */
		printf("%d %s HTTP/%u.%u\r\n", response, response_text(response), HTTP_MAJOR_VERSION, HTTP_MINOR_VERSION);

		/* header options */
		spewoutputoptions();
		fflush(stdout);

		ssize_t content_length = atoi(getoption("Content-Length"));
		if (content_length > 0)
			if (write(1, content, content_length) != content_length)
				fprintf(stderr, "could not output %d bytes of content\n", content_length);

		currentver++;
	} while (!terminate); /* main loop */

	return 0;
}
