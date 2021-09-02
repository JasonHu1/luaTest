#ifndef __HAL_COMM_H__
#define __HAL_COMM_H__
#if 1
#include "app_debug_printf.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ty_cJSON.h"
#include "random-test-server.h"
#include <pthread.h>
#endif
#define BUS_PROTOCOL_MODBUS         1
#define BUS_PROTOCOL_KNX            2

typedef struct _uartcfg{
    uint8 channel;
    uint8 busProto;
    uint8 devName[32];
    uint32 baud;
    uint8 parity;
    uint8 dataBit;
    uint8 stopBit;
}UARTCFG_T;

typedef struct _commInfo{
    UARTCFG_T*pCfg;
    void *pConnCxt;
}COMM_INFO_T;

typedef struct _queueElement {
  struct _queueElement* next;
  void* content;
} QUEUE_Element_t;

typedef struct _queueHandler {
  QUEUE_Element_t* head;
  QUEUE_Element_t* tail;
  pthread_mutex_t mutex_queue;
  int nb;
} QUEUE_MANAGER_T;

QUEUE_MANAGER_T* queue_init(void);
int queue_pushback(QUEUE_MANAGER_T * queue, void * content);
int queue_popfront(QUEUE_MANAGER_T*queue,void**content);

COMM_INFO_T* user_get_uartConfigure(CHAR_T *cnt);
COMM_INFO_T* user_get_conn_context_byChannel(CHAR_T channel);
int user_save_uartConfigure(IN CONST CHAR_T *str_cfg);



#endif//__HAL_COMM_H__
