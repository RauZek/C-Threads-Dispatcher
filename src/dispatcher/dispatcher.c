#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dispatcher.h"

typedef struct {
    base_thread_t* dispatcher;
    base_thread_t* sp; // Simple process
    base_thread_t* ap; // Advanced process

    void* sp_lib; // Simple process library
    void* ap_lib; // Advanced process library

    uint32_t comb_msgs[33]; // Combined messages
    size_t comb_size; // Combined messages sizes
} dispatcher_t;

void dispatcher_process(void* instance, uint32_t msg) {
    dispatcher_t* dispatcher = (dispatcher_t*)instance;
    bool is_sent = false;

    base_process* sp = (base_process*)dispatcher->sp->instance;
    base_process* ap = (base_process*)dispatcher->ap->instance;

    for (size_t i = 0; i < sp->msg_size; i++) {
        if (sp->supported_msgs[i] == msg) {
            send_msg(dispatcher->sp, msg);
            is_sent = true;
            break;
        }
    }

    for (size_t i = 0; i < ap->msg_size; i++) {
        if (ap->supported_msgs[i] == msg) {
            send_msg(dispatcher->ap, msg);
            is_sent = true;
            break;
        }
    }

    if (!is_sent) {
        fprintf(stderr, "Dispatcher received an unsupported message: %u\n", msg);
    }
}

uint32_t dispatcher_init(void) {
    dispatcher_t* dispatcher = calloc(1, sizeof(dispatcher_t));

    if (!dispatcher) {
        printf("Error initialising the dispatcher.");
        return -1;
    }

    base_process* (*sp_fn)();
    base_thread_t* (*sp_init_fn)();

    base_process* (*ap_fn)();
    base_thread_t* (*ap_init_fn)();

    dispatcher->sp_lib = dlopen("libc_simple_process.so", RTLD_LAZY);
    dispatcher->ap_lib = dlopen("libc_advanced_process.so", RTLD_LAZY);

    if (!dispatcher->sp_lib) {
        fprintf(stderr, "the dlopen failed: %s\n", dlerror());
        dlclose(dispatcher->sp_lib);

        return -1;
    }

    if (!dispatcher->ap_lib) {
        fprintf(stderr, "the dlopen failed: %s\n", dlerror());
        dlclose(dispatcher->ap_lib);

        return -1;
    }

    *(void**)&sp_fn = dlsym(dispatcher->sp_lib, "simple_process_functions");
    *(void**)&sp_init_fn = dlsym(dispatcher->sp_lib, "sp_init");

    *(void**)&ap_fn = dlsym(dispatcher->ap_lib, "advanced_process_functions");
    *(void**)&ap_init_fn = dlsym(dispatcher->ap_lib, "ap_init");

    if (!sp_fn || !sp_init_fn || !ap_fn || !ap_init_fn) {
        fprintf(stderr, "the dlsym failed: %s\n", dlerror());
        dlclose(dispatcher->sp_lib);
        dlclose(dispatcher->ap_lib);
        dispatcher_destroy();

        return -1;
    }

    base_process* sp_proc = sp_fn();
    base_process* ap_proc = ap_fn();

    dispatcher->sp = sp_init_fn();
    dispatcher->ap = ap_init_fn();

    uint32_t* sp_supported_msgs = NULL;
    uint32_t* ap_supported_msgs = NULL;
    size_t sp_size = 0, ap_size = 0;

    sp_proc->get_supported_msg(&sp_supported_msgs, &sp_size);
    ap_proc->get_supported_msg(&ap_supported_msgs, &ap_size);

    dispatcher->comb_size = 0;

    for (size_t i = 0; i < sp_size; i++) {
        dispatcher->comb_msgs[dispatcher->comb_size++] = sp_supported_msgs[i];
    }

    for (size_t i = 0; i < ap_size; i++) {
        bool duplicate = false;
        for (size_t j = 0; j < sp_size; j++) {
            if (ap_supported_msgs[i] == sp_supported_msgs[j]) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate && (dispatcher->comb_size < 33)) {
            dispatcher->comb_msgs[dispatcher->comb_size++] = ap_supported_msgs[i];
        }
    }

    // Inits the dispatcher's thread.
    dispatcher->dispatcher = init(dispatcher_process);
    if (!dispatcher->dispatcher) {
        fprintf(stderr, "Failed to initialize dispatcher thread\n");
        dispatcher_destroy();

        return -1;
    }

    free(sp_supported_msgs);
    free(ap_supported_msgs);

    return 0;
}

void dispatcher_get_supported_msg(uint32_t** msgs, size_t* size) {
    dispatcher_t* dispatcher = calloc(1, sizeof(dispatcher_t));

    if (!dispatcher) {
        *msgs = NULL;
        *size = 0;

        return -1;
    }

    *msgs = malloc((dispatcher->comb_size) * (sizeof(uint32_t)));

    if (*msgs) {
        memcpy(*msgs, dispatcher->comb_msgs, dispatcher->comb_size * sizeof(uint32_t));
        *size = dispatcher->comb_size;
    } else {
        *size = 0;
    }
}

void dispatcher_destroy(void) {
    dispatcher_t* dispatcher = calloc(1, sizeof(dispatcher_t));

    if (dispatcher) {
        if (dispatcher->sp) {
            destroy(dispatcher->sp);
            dispatcher->sp = NULL;
        }
        if (dispatcher->ap) {
            destroy(dispatcher->ap);
            dispatcher->ap = NULL;
        }
        if (dispatcher->sp_lib) {
            dlclose(dispatcher->sp_lib);
            dispatcher->sp_lib = NULL;
        }
        if (dispatcher->ap_lib) {
            dlclose(dispatcher->ap_lib);
            dispatcher->ap_lib = NULL;
        }
        if (dispatcher->dispatcher) {
            destroy(dispatcher->dispatcher);
            dispatcher->dispatcher = NULL;
        }

        free(dispatcher);
    }
}

base_process* dispatcher_functions(void) {
    base_process* dispatcher_functions = calloc(1, sizeof(base_process));
    dispatcher_functions->process = &dispatcher_process;
    dispatcher_functions->get_supported_msg = &dispatcher_get_supported_msg;

    return dispatcher_functions;
}
