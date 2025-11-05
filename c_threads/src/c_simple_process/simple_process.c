#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "simple_process.h"


static void sp_process(void* instance, uint32_t msg) {
    base_process_t* sp = (base_process_t*)instance;
    bool is_supported = false;

    for (size_t i = 0; i < sp->msg_size; i++) {
        if (sp->supported_msgs[i] == msg) {
            is_supported = true;
            break;
        }
    }

    if (is_supported) {
        printf("SIMPLE_PROCESS: Received supported message: %u\n", msg);
    } else {
        fprintf(stderr, "SIMPLE_PROCESS: Error! Received unsupported message: %u\n", msg);
    }
}

static void sp_get_supported_msg(void* instance) {
    base_process_t* sp = (base_process_t*)instance;

    const uint32_t SP_SUPPORTED_MSGS[] = {
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60
    };
    const size_t SP_MSG_COUNT = sizeof(SP_SUPPORTED_MSGS) / sizeof(SP_SUPPORTED_MSGS[0]);

    for (size_t i = 0; i < SP_MSG_COUNT; i++) {
        sp->supported_msgs[i] = SP_SUPPORTED_MSGS[i];
    }
    
    sp->msg_size = SP_MSG_COUNT;
}

base_process_t* simple_process_functions(void) {
    static base_process_t sp_functions = {
        .process = &sp_process,
        .get_supported_msg = &sp_get_supported_msg
    };

    return &sp_functions;
}
