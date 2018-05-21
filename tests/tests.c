#include "test.h"
#include <stdio.h>

int scan_http_headers();

int main()
{
    test(scan_http_headers, "HTTP Headers");

    printf("DONE: %d/%d\n", num_tests_passed, num_tests_passed + num_tests_failed);
    return num_tests_failed > 0;
}