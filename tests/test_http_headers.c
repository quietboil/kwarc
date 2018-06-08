#include "test.h"
#include "http_headers.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**
 * Skips the rest of the header line either becuase the header is not recognized, i.e.
 * it is ignorable or to skip the payload (we are not dealing with payloads in this test).
 * \param  ptr  Pointer to the current position on a line.
 * \param  end  Pointer to the end of the text buffer.
 * \return Pointer to the first character on the next line or `end`.
 */
static const char * skip_line(const char * ptr, const char * end)
{
    while (ptr < end && *ptr++ != '\n') {}
    return ptr;
}

int scan_http_headers()
{
    const char http[] =
        "Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==\r\n"
        "Accept: text/plain\r\n"
        "Accept-Charset: utf-8\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Accept-Language: en-US\r\n"
        "Accept-Datetime: Thu, 31 May 2007 20:35:00 GMT\r\n"
        "Content-Length: 348\r\n"
    ;
    const char * ptr = http;
    const char * end = http + sizeof(http) - 1;

    http_headers_scan_result_t result;

    result = http_headers_scan(0, ptr, end);

    check(result.state == 0); // Authorization
    ptr = skip_line(ptr + result.length, end);
    check(ptr < end);

    result = http_headers_scan(0, ptr, end);
    check(result.state == ACCEPT);
    ptr = skip_line(ptr + result.length, end);
    check(ptr < end);

    result = http_headers_scan(0, ptr, end);
    check(result.state == ACCEPT_CHARSET);
    ptr = skip_line(ptr + result.length, end);
    check(ptr < end);

    result = http_headers_scan(0, ptr, end);
    check(result.state == ACCEPT_ENCODING);
    ptr = skip_line(ptr + result.length, end);
    check(ptr < end);

    result = http_headers_scan(0, ptr, end);
    check(result.state == ACCEPT_LANGUAGE);
    ptr = skip_line(ptr + result.length, end);
    check(ptr < end);

    result = http_headers_scan(0, ptr, end);
    check(result.state == ACCEPT_DATETIME);
    ptr = skip_line(ptr + result.length, end);
    check(ptr < end);

    result = http_headers_scan(0, ptr, end);
    check(result.state == 0); // Content-Length
    ptr = skip_line(ptr + result.length, end);
    check(ptr == end);

    return 0;
}