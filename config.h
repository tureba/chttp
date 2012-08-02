#ifndef CONFIG_H
#define CONFIG_H

#define SERVER_ID "chttp/0.0"

/* HTTP/1.1 */
#define HTTP_MAJOR_VERSION 1
#define HTTP_MINOR_VERSION 1

/* The longest method in RFCs I found has 17 chars. */
#define METHOD_SIZE_TEXT "17"
#define METHOD_SIZE_INT 17 + 1

#define ADDR_SIZE_TEXT "2047"
#define ADDR_SIZE_INT 2047 + 1

#endif /* config_h */
