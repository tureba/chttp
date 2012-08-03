#ifndef CONFIG_H
#define CONFIG_H

#define SERVER_ID "chttp/0.0"

#define INTTOSTR(s) TEXTTOSTR(s)
#define TEXTTOSTR(s) #s

/* HTTP/1.1 */
#define HTTP_MAJOR_VERSION 1
#define HTTP_MINOR_VERSION 1

/* The longest method in RFCs I found has 17 chars. */
#define METHOD_SIZE 17

/* RFC2616 says I can choose an arbitrary maximum address
 * length and return 414 on large addresses. Is 2047 too
 * much or too little? I should think about alocating this
 * dynamically. */
#define ADDR_SIZE 2047

#define KEY_SIZE 49

#define VALUE_SIZE 2047

#endif /* config_h */
