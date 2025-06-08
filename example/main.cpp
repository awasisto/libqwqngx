#include "qwqngx.h"

#include <cstdio>
#include <cstdlib>

int main()
{
    qwqngx* q = nullptr;
    if (qwqngx_init(&q) != QWQNGX_RESULT_OK) {
        printf("Error initializing qwqngx\n");
        return EXIT_FAILURE;
    }

    printf("Get 10 bytes of random data\n");
    char buffer[10];
    if (qwqngx_randbytes(q, buffer, 10) != QWQNGX_RESULT_OK) {
        printf("Error: %s\n", qwqngx_status_string(q));
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 10; ++i) {
        printf("%02X ", buffer[i] & 0xFF);
    }
    printf("\n");

    qwqngx_free(q);

    return EXIT_SUCCESS;
}
