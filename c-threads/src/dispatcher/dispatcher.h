#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include <stdint.h>
#include <stddef.h>

int dispatcher_init(void);
void dispatcher_get_supported_msg(uint32_t** msgs, size_t* msg_size);
void dispatcher_send_msg(uint32_t msg);
void dispatcher_destroy(void);

#endif //  __DISPATCHER_H__
