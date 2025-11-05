#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dispatcher.h"

int main(int argc, char** argv) {
    uint32_t* supported_msgs = NULL;
    size_t supported_size = 0;
    bool validation_failed = false;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <msg1> [msg2] [...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("APP: Initializing dispatcher...\n");
    if (dispatcher_init() != 0) {
        fprintf(stderr, "APP: Failed to initialize dispatcher.\n");
        return EXIT_FAILURE;
    }

    dispatcher_get_supported_msg(&supported_msgs, &supported_size);
    if (!supported_msgs || supported_size == 0) {
        fprintf(stderr,
                "APP: Failed to get supported message list from dispatcher.\n");
        dispatcher_destroy();
        return EXIT_FAILURE;
    }

    printf("APP: Dispatcher supports %zu messages:\n", supported_size);
    for (size_t i = 0; i < supported_size; i++) {
        printf("%u ", supported_msgs[i]);
    }
    printf("\n");

    printf("APP: Validating arguments...\n");
    for (int i = 1; i < argc; i++) {
        char* end = NULL;
        long val = strtol(argv[i], &end, 10);

        if (end == argv[i] || *end != '\0' || val < 0) {
            fprintf(stderr, "Error: Invalid input argument: '%s'\n", argv[i]);
            validation_failed = true;
            break;
        }

        uint32_t msg = (uint32_t)val;

        if (msg == 0) {
            fprintf(stderr,
                    "Error: Message '0' is reserved and cannot be passed via "
                    "CLI.\n");
            validation_failed = true;
            break;
        }

        bool is_supported = false;
        for (size_t j = 0; j < supported_size; j++) {
            if (supported_msgs[j] == msg) {
                is_supported = true;
                break;
            }
        }

        if (!is_supported) {
            fprintf(stderr, "Error: Unsupported message: %u\n", msg);
            fprintf(stderr, "Program will exit.\n");
            validation_failed = true;
            break;
        }
    }

    if (!validation_failed) {
        printf("APP: All arguments validated. Sending %d messages...\n",
               argc - 1);

        for (int i = 1; i < argc; i++) {
            uint32_t msg = (uint32_t)strtoul(argv[i], NULL, 10);
            printf("APP: Sending message %u\n", msg);

            dispatcher_send_msg(msg);
        }
        printf("APP: All messages sent.\n");
    } else {
        fprintf(stderr,
                "APP: Input validation failed. No messages will be sent.\n");
    }

    printf("APP: Shutting down...\n");
    dispatcher_destroy();

    if (validation_failed) {
        return EXIT_FAILURE;
    }

    printf("APP: Exiting successfully.\n");
    return EXIT_SUCCESS;
}
