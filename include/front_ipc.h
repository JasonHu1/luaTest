#ifndef __FRONT_IPC_H__
#define __FRONT_IPC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
#include "ipc_stub.h"
#ifdef __cplusplus
}
#endif//__cplusplus

void MsgHandler(void *msgData);

#endif//__FRONT_IPC_H__