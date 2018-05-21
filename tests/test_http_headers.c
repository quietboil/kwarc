#include "test.h"
#include "http_headers.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**
 * Scans the header line to determine the header type and position of the payload.
 * \param       ptr   Pointer to the first character on the current line.
 * \param       end   Pointer to the end of the text buffer.
 * \param[out]  type  Pointer to the result variable where the recognized header type will be stored.
 * \return Pointer to the `:` character if the header is recognized and `*type` is returned,
 *         Pointer to a character at which automaton failed to recignized the header (`*type` is returned as 0)
 *         `end` when end of text buffer is reached before the header is recognized.
 */
static const char * scan_http_header(const char * ptr, const char * end, uint8_t * type)
{
    uint16_t state = 0;
    while (ptr < end) {
        state = http_headers_next_state(state, *ptr++);
        if (state == 0)
            break;
        if (state < UINT8_MAX && *ptr == ':')
            break;
    }
    *type = state;
    return ptr;
}

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
    uint8_t header_type;
    const char * end = http + sizeof(http) - 1;

    const char * ptr = scan_http_header(http, end, &header_type);
    check(header_type == 0); // Authorization
    ptr = skip_line(ptr, end);
    check(ptr < end);

    ptr = scan_http_header(ptr, end, &header_type);
    check(header_type == ACCEPT);
    ptr = skip_line(ptr, end);
    check(ptr < end);

    ptr = scan_http_header(ptr, end, &header_type);
    check(header_type == ACCEPT_CHARSET);
    ptr = skip_line(ptr, end);
    check(ptr < end);

    ptr = scan_http_header(ptr, end, &header_type);
    check(header_type == ACCEPT_ENCODING);
    ptr = skip_line(ptr, end);
    check(ptr < end);

    ptr = scan_http_header(ptr, end, &header_type);
    check(header_type == ACCEPT_LANGUAGE);
    ptr = skip_line(ptr, end);
    check(ptr < end);

    ptr = scan_http_header(ptr, end, &header_type);
    check(header_type == ACCEPT_DATETIME);
    ptr = skip_line(ptr, end);
    check(ptr < end);

    ptr = scan_http_header(ptr, end, &header_type);
    check(header_type == 0); // Content-Length
    ptr = skip_line(ptr, end);
    check(ptr == end);

    return 0;
}