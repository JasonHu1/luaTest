#ifndef __TIME_SCALE_H__
#define __TIME_SCALE_H__
#include "app_defs_types.h"
#include "app_debug_printf.h"

typedef int (*timerfunctionCB)(void*param);
typedef enum {
    TIMER_SINGLE,
    TIMER_REPEATABLE,
}TIMESCALE_MODE_T;

typedef struct _timescale{
    uint32 timerId;
    timerfunctionCB cb;
    uint32 timeout;
    uint32 Reloads;
    void *cb_param;
    TIMESCALE_MODE_T mode;
}TIMESCALE_PAYLOAD_T;

typedef struct _TimeScaleListElement {
  struct _TimeScaleListElement* next;
  struct _TimeScaleListElement* previous;
  TIMESCALE_PAYLOAD_T* content;
} TimeScaleListElement_t;

int timescale_init(void);
int timescale_create(uint32 timeout,void *cb_param,TIMESCALE_MODE_T mode,timerfunctionCB funcb);
void * timescale_task_loop(void*args);
int timer_60s_cb(void * param);

#endif//__TIME_SCALE_H__