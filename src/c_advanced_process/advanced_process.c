#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "advanced_process.h"

void process(void* instance, uint32_t msg) {
    base_process* ap = (base_process*)instance;

    uint32_t* supported_msgs = NULL;
    size_t msg_size = 0;
    bool is_supported = false;

    ap->get_supported_msg(&supported_msgs, &msg_size);

    for (size_t i = 0; i < msg_size; i++) {
        if (supported_msgs[i] == msg) {
            is_supported = true;
            break;
        }
    }

    if (is_supported) {
        printf("The advanced process received the supported msg: %u\n", msg);
    } else {
        fprintf(stderr, "Error: the advanced process received an unsupported msg: %u\n", msg);
    }
}

void get_supported_msg(uint32_t** msgs, size_t* msg_size) {
    uint32_t ap_supported_msgs[] = {
        30,31,32,33,34,35,36,37,38,39,40,
        50,51,52,53,54,55,56,57,58,59,60
    };

    base_process* base_process = advanced_process_functions();
    memcpy(base_process->supported_msgs, ap_supported_msgs, sizeof(ap_supported_msgs));
    base_process->msg_size = sizeof(ap_supported_msgs) / sizeof(ap_supported_msgs[0]);

    *msgs = base_process->supported_msgs;
    *msg_size = base_process->msg_size;
}

base_process* advanced_process_functions() {
    base_process* advanced_process_functions = calloc(1, sizeof(base_process));
    advanced_process_functions->process = &process;
    advanced_process_functions->get_supported_msg = &get_supported_msg;

    return advanced_process_functions;
}

base_thread_t* ap_init() {
    base_process* ap = advanced_process_functions();
    ap->thread = init(process);

    return ap->thread;
}
