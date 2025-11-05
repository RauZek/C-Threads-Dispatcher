#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "advanced_process.h"

static void ap_process(void* instance, uint32_t msg) {
    base_process_t* ap = (base_process_t*)instance;
    bool is_supported = false;

    for (size_t i = 0; i < ap->msg_size; i++) {
        if (ap->supported_msgs[i] == msg) {
            is_supported = true;
            break;
        }
    }

    if (is_supported) {
        printf("ADVANCED_PROCESS: Received supported message: %u\n", msg);
    } else {
        fprintf(stderr,
                "ADVANCED_PROCESS: Error! Received unsupported message: %u\n",
                msg);
    }
}

static void ap_get_supported_msg(void* instance) {
    base_process_t* ap = (base_process_t*)instance;

    const uint32_t AP_SUPPORTED_MSGS[] = {30, 31, 32, 33, 34, 35, 36, 37,
                                          38, 39, 40, 50, 51, 52, 53, 54,
                                          55, 56, 57, 58, 59, 60};
    const size_t AP_MSG_COUNT =
        sizeof(AP_SUPPORTED_MSGS) / sizeof(AP_SUPPORTED_MSGS[0]);

    for (size_t i = 0; i < AP_MSG_COUNT; i++) {
        ap->supported_msgs[i] = AP_SUPPORTED_MSGS[i];
    }

    ap->msg_size = AP_MSG_COUNT;
}

base_process_t* advanced_process_functions(void) {
    static base_process_t ap_functions = {
        .process = &ap_process, .get_supported_msg = &ap_get_supported_msg};

    return &ap_functions;
}