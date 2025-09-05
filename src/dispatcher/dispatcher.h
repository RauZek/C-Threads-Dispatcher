#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include <stdint.h>
#include "../c_base_thread/base_thread.h"

uint32_t dispatcher_init();
void dispatcher_process(void* instance, uint32_t msg);
void dispatcher_destroy(void);
void dispatcher_get_supported_msg(uint32_t** msgs, size_t* msg_size);
base_process* dispatcher_functions(void);

#endif //  __DISPATCHER_H__
