#ifndef STATUSCODES_H
#define STATUSCODES_H

const char *response_texts[5][32] =
	{
		{
/* 100 */ "Continue",	/* RFC 2616 */
/* 101 */ "Switching protocols",	/* RFC 2616 */
/* 102 */ "Processing",	/* RFC 2518 */
		},
		{
/* 200 */ "Ok",
/* 201 */ "Created",
/* 202 */ "Accepted",
/* 203 */ "Non-authoritive information",
/* 204 */ "No content",
/* 205 */ "Reset content",
/* 206 */ "Partial content",
/* 207 */ "???",
/* 208 */ "???",
/* 209 */ "???",
/* 210 */ "???",
/* 211 */ "???",
/* 212 */ "???",
/* 213 */ "???",
/* 214 */ "???",
/* 215 */ "???",
/* 216 */ "???",
/* 217 */ "???",
/* 218 */ "???",
/* 219 */ "???",
/* 220 */ "???",
/* 221 */ "???",
/* 222 */ "???",
/* 223 */ "???",
/* 224 */ "???",
/* 225 */ "???",
/* 226 */ "IM used",
		},
		{
/* 300 */ "Multiple choices",
/* 301 */ "Moved permanently",
/* 302 */ "Moved temporarily",
/* 303 */ "See other",
/* 304 */ "Not modified",
/* 305 */ "Use proxy",
		},
		{
/* 400 */ "Bad request",
/* 401 */ "Unauthorized",
/* 402 */ "Payment required",
/* 403 */ "Forbidden",
/* 404 */ "Not found",
/* 405 */ "Method not allowed",
/* 406 */ "Not acceptable",
/* 407 */ "Proxy authentication required",
/* 408 */ "Request timeout",
/* 409 */ "Conflict",
/* 410 */ "Gone",
/* 411 */ "Length required",
/* 412 */ "Precondition failed",
/* 413 */ "Request entity too large",
/* 414 */ "Request URI too large",
/* 415 */ "Unsupported media type",
/* 416 */ "???",
/* 417 */ "???",
/* 418 */ "???",
/* 419 */ "???",
/* 420 */ "???",
/* 421 */ "???",
/* 422 */ "???",
/* 423 */ "???",
/* 424 */ "???",
/* 425 */ "???",
/* 426 */ "Upgrade Required",
/* 427 */ "???",
/* 428 */ "Precondition Required",	/* RFC 6585 */
/* 429 */ "Too Many Requests",	/* RFC 6585 */
/* 430 */ "???",
/* 431 */ "Request Header Fields Too Large",	/* RFC 6585 */
		},
		{
/* 500 */ "Internal server error",	/* RFC 2616 */
/* 501 */ "Not implemented",	/* RFC 2616 */
/* 502 */ "Bad gateway",	/* RFC 2616 */
/* 503 */ "Service unavailable",	/* RFC 2616 */
/* 504 */ "Gateway timeout",	/* RFC 2616 */
/* 505 */ "HTTP version not supported",	/* RFC 2616 */
/* 506 */ "Variant Also Negotiates (Experimental)",	/* RFC 2295 */
/* 507 */ "Insufficient Storage",	/* RFC 4918 */
/* 508 */ "Loop Detected",	/* RFC 5842 */
/* 509 */ "???",
/* 510 */ "Not Extended",	/* RFC 2774 */
/* 511 */ "Network Authentication Required",	/* RFC 6585 */
		}
	};

#define response_text(n) (response_texts[n/100-1][n%100])

#endif /* STATUSCODES_H */
