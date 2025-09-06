#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dispatcher/dispatcher.h"

int main(int argc, char** argv) {
    uint32_t* supported_msgs = NULL;
    size_t supported_size = 0;
    size_t valid_size = 0;
    uint32_t msg = 0, i = 0, j = 0;
    bool is_supported = false;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <msg1> [msg2] [...]\n", argv[0]);

        return EXIT_FAILURE;
    }

    if (dispatcher_init() != 0) {
        fprintf(stderr, "Failed to initialize dispatcher\n");

        return EXIT_FAILURE;
    }

    dispatcher_get_supported_msg(&supported_msgs, &supported_size);

    uint32_t* valid_msgs = calloc(argc - 1, sizeof(*valid_msgs));
    if (!valid_msgs) {
        fprintf(stderr, "calloc failed: %s\n", strerror(errno));

        return EXIT_FAILURE;
    }

    for (i = 1; i < (uint32_t)argc; i++) {
        is_supported = false;
        msg = (uint32_t)atoi(argv[i]);

        if (0 == msg) {
            fprintf(stderr,"ERROR: Message 0 is reserved and cannot be passed via CLI\n\n");
            free(valid_msgs);
            dispatcher_destroy();

            return -1;
        }

        for (j = 0; j < supported_size; j++) {
            if (supported_msgs[j] == msg) {
                is_supported = true;
                break;
            }
        }

        if (is_supported) {
            valid_msgs[valid_size++] = msg;
        } else {
            fprintf(stderr, "Unsupported message: %u\n\n", msg);
        }
    }

    dispatcher_process(valid_msgs, (uint32_t)argc - 1);
    free(valid_msgs);
    dispatcher_destroy();

    return EXIT_SUCCESS;
}
