#include "dispatcher.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "advanced_process.h"
#include "base_thread.h"
#include "simple_process.h"

#define SIMPLE_PROCESS_LIB "libc_simple_process.so"
#define SIMPLE_PROCESS_FUNC "simple_process_functions"
#define ADVANCED_PROCESS_LIB "libc_advanced_process.so"
#define ADVANCED_PROCESS_FUNC "advanced_process_functions"

#define MAX_COMBINED_MSGS 33

typedef struct {
    base_thread_t* thread;

    void* sp_lib_handle;
    void* ap_lib_handle;

    base_process_t* sp_inst;
    base_process_t* ap_inst;

    uint32_t combined_msgs[MAX_COMBINED_MSGS];
    size_t combined_size;
} dispatcher_t;

static dispatcher_t g_dispatcher;
typedef base_process_t* (*get_process_fn_t)(void);

static void dispatcher_process(void* instance, uint32_t msg) {
    dispatcher_t* d = (dispatcher_t*)instance;
    bool sent_to_sp = false;
    bool sent_to_ap = false;

    for (size_t i = 0; i < d->sp_inst->msg_size; i++) {
        if (d->sp_inst->supported_msgs[i] == msg) {
            send_msg(d->sp_inst->thread, msg);
            sent_to_sp = true;
            break;
        }
    }

    for (size_t i = 0; i < d->ap_inst->msg_size; i++) {
        if (d->ap_inst->supported_msgs[i] == msg) {
            send_msg(d->ap_inst->thread, msg);
            sent_to_ap = true;
            break;
        }
    }

    if (!sent_to_sp && !sent_to_ap) {
        fprintf(
            stderr,
            "DISPATCHER: Error! Received msg %u, but no process supports it.\n",
            msg);
    }
}

static void dispatcher_build_combined_list(void) {
    dispatcher_t* d = &g_dispatcher;
    d->combined_size = 0;

    for (size_t i = 0; i < d->sp_inst->msg_size; i++) {
        if (d->combined_size < MAX_COMBINED_MSGS) {
            d->combined_msgs[d->combined_size++] =
                d->sp_inst->supported_msgs[i];
        }
    }

    for (size_t i = 0; i < d->ap_inst->msg_size; i++) {
        bool is_duplicate = false;
        uint32_t ap_msg = d->ap_inst->supported_msgs[i];

        for (size_t j = 0; j < d->sp_inst->msg_size; j++) {
            if (ap_msg == d->sp_inst->supported_msgs[j]) {
                is_duplicate = true;
                break;
            }
        }

        if (!is_duplicate && d->combined_size < MAX_COMBINED_MSGS) {
            d->combined_msgs[d->combined_size++] = ap_msg;
        }
    }

    printf("Dispatcher: Combined %zu unique messages.\n", d->combined_size);
}

int dispatcher_init(void) {
    g_dispatcher.sp_lib_handle = dlopen(SIMPLE_PROCESS_LIB, RTLD_LAZY);
    if (!g_dispatcher.sp_lib_handle) {
        fprintf(stderr, "DISPATCHER: Failed to dlopen %s: %s\n",
                SIMPLE_PROCESS_LIB, dlerror());
        return -1;
    }

    g_dispatcher.ap_lib_handle = dlopen(ADVANCED_PROCESS_LIB, RTLD_LAZY);
    if (!g_dispatcher.ap_lib_handle) {
        fprintf(stderr, "DISPATCHER: Failed to dlopen %s: %s\n",
                ADVANCED_PROCESS_LIB, dlerror());
        dlclose(g_dispatcher.sp_lib_handle);
        return -1;
    }

    get_process_fn_t sp_func;
    get_process_fn_t ap_func;
    char* error;

    dlerror();
    *(void**)(&sp_func) =
        dlsym(g_dispatcher.sp_lib_handle, SIMPLE_PROCESS_FUNC);

    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "DISPATCHER: Failed to dlsym %s: %s\n",
                SIMPLE_PROCESS_FUNC, error);
        dispatcher_destroy();
        return -1;
    }

    dlerror();
    *(void**)(&ap_func) =
        dlsym(g_dispatcher.ap_lib_handle, ADVANCED_PROCESS_FUNC);

    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "DISPATCHER: Failed to dlsym %s: %s\n",
                ADVANCED_PROCESS_FUNC, error);
        dispatcher_destroy();
        return -1;
    }

    base_process_t* sp_funcs = sp_func();
    base_process_t* ap_funcs = ap_func();

    g_dispatcher.sp_inst = calloc(1, sizeof(base_process_t));
    g_dispatcher.ap_inst = calloc(1, sizeof(base_process_t));
    if (!g_dispatcher.sp_inst || !g_dispatcher.ap_inst) {
        fprintf(stderr, "DISPATCHER: Failed to calloc process instances.\n");
        dispatcher_destroy();
        return -1;
    }

    g_dispatcher.sp_inst->process = sp_funcs->process;
    g_dispatcher.sp_inst->get_supported_msg = sp_funcs->get_supported_msg;

    g_dispatcher.ap_inst->process = ap_funcs->process;
    g_dispatcher.ap_inst->get_supported_msg = ap_funcs->get_supported_msg;

    g_dispatcher.sp_inst->get_supported_msg(g_dispatcher.sp_inst);
    g_dispatcher.ap_inst->get_supported_msg(g_dispatcher.ap_inst);

    printf("DISPATCHER: Simple Process supports %zu messages.\n",
           g_dispatcher.sp_inst->msg_size);
    printf("DISPATCHER: Advanced Process supports %zu messages.\n",
           g_dispatcher.ap_inst->msg_size);

    g_dispatcher.sp_inst->thread =
        init(g_dispatcher.sp_inst->process, g_dispatcher.sp_inst);
    g_dispatcher.ap_inst->thread =
        init(g_dispatcher.ap_inst->process, g_dispatcher.ap_inst);

    if (!g_dispatcher.sp_inst->thread || !g_dispatcher.ap_inst->thread) {
        fprintf(stderr, "DISPATCHER: Failed to init process threads.\n");
        dispatcher_destroy();
        return -1;
    }

    dispatcher_build_combined_list();

    g_dispatcher.thread = init(dispatcher_process, &g_dispatcher);
    if (!g_dispatcher.thread) {
        fprintf(stderr, "DISPATCHER: Failed to init dispatcher thread.\n");
        dispatcher_destroy();
        return -1;
    }

    printf("DISPATCHER: Init complete. All threads running.\n");
    return 0;
}

void dispatcher_get_supported_msg(uint32_t** msgs, size_t* msg_size) {
    *msgs = g_dispatcher.combined_msgs;
    *msg_size = g_dispatcher.combined_size;
}

void dispatcher_send_msg(uint32_t msg) {
    if (g_dispatcher.thread) {
        send_msg(g_dispatcher.thread, msg);
    }
}

void dispatcher_destroy(void) {
    printf("DISPATCHER: Shutting down...\n");

    if (g_dispatcher.thread) {
        destroy(g_dispatcher.thread);
        g_dispatcher.thread = NULL;
    }
    if (g_dispatcher.sp_inst && g_dispatcher.sp_inst->thread) {
        destroy(g_dispatcher.sp_inst->thread);
    }
    if (g_dispatcher.ap_inst && g_dispatcher.ap_inst->thread) {
        destroy(g_dispatcher.ap_inst->thread);
    }

    free(g_dispatcher.sp_inst);
    free(g_dispatcher.ap_inst);
    g_dispatcher.sp_inst = NULL;
    g_dispatcher.ap_inst = NULL;

    if (g_dispatcher.sp_lib_handle) {
        dlclose(g_dispatcher.sp_lib_handle);
        g_dispatcher.sp_lib_handle = NULL;
    }
    if (g_dispatcher.ap_lib_handle) {
        dlclose(g_dispatcher.ap_lib_handle);
        g_dispatcher.ap_lib_handle = NULL;
    }

    printf("DISPATCHER: Shutdown complete.\n");
}
