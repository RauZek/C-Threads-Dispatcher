#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../c_simple_process/simple_process.h"

void process(void* instance, uint32_t msg) {
    base_process* sp = (base_process*)instance;

    uint32_t* supported_msgs = NULL;
    size_t msg_size = 0;
    bool is_supported = false;

    sp->get_supported_msg(&supported_msgs, &msg_size);

    for (size_t i = 0; i < msg_size; i++) {
        if (supported_msgs[i] == msg) {
            is_supported = true;
            break;
        }
    }

    if (is_supported) {
        printf("The simple process received the supported msg: %u\n", msg);
    } else {
        fprintf(stderr, "Error: the simple process received an unsupported msg: %u\n", msg);
    }
}

void get_supported_msg(uint32_t** msgs, size_t* msg_size) {
    uint32_t sp_supported_msgs[] = {
        10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 50, 51, 52, 53, 54,
        55, 56, 57, 58, 59, 60
    };

    base_process* base_process = simple_process_functions();
    memcpy(base_process->supported_msgs, sp_supported_msgs, sizeof(sp_supported_msgs));
    base_process->msg_size = sizeof(sp_supported_msgs) / sizeof(sp_supported_msgs[0]);

    *msgs = base_process->supported_msgs;
    *msg_size = base_process->msg_size;
}

base_process* simple_process_functions() {
    base_process* simple_process_functions = calloc(1, sizeof(base_process));
    simple_process_functions->process = &process;
    simple_process_functions->get_supported_msg = &get_supported_msg;

    return simple_process_functions;
}

base_thread_t* sp_init() {
    base_process* sp = simple_process_functions();
    sp->thread = init(process);

    return sp->thread;
}
