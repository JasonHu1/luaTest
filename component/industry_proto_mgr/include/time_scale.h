#ifndef __TIME_SCALE_H__
#define __TIME_SCALE_H__
#include "uni_log.h"
#include "modbus.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#ifdef __cplusplus
}
#endif



typedef int (*timerfunctionCB)(void*param);
typedef enum {
    TIMER_SINGLE,
    TIMER_REPEATABLE,
}TIMESCALE_MODE_T;

typedef struct _timescale{
    uint32_t timerId;
    timerfunctionCB cb;
    uint32_t timeout;
    uint32_t Reloads;
    void *cb_param;
    TIMESCALE_MODE_T mode;
}TIMESCALE_PAYLOAD_T;

typedef struct _TimeScaleListElement {
  struct _TimeScaleListElement* next;
  struct _TimeScaleListElement* previous;
  TIMESCALE_PAYLOAD_T* content;
} TimeScaleListElement_t;

typedef struct _luaAttr{
    int functionCode;
    int addr;
    int count;
}LUAATTR_T;

typedef struct _modbusFunctionMap{
    int (*read_bits)(modbus_t *, int , int , uint8_t *);                    //0x01
    int (*read_input_bits)(modbus_t *, int , int , uint8_t *);              //0x02
    int (*read_registers)(modbus_t *, int , int , uint16_t *);              //0x03
    int (*read_input_registers)(modbus_t *, int , int , uint16_t *);        //0x04
    int (*write_bit)(modbus_t *, int , int );                               //0x05
    int (*write_register)(modbus_t *, int , const uint16_t );               //0x06
    void(*no_function_07)(void*);                                           //0x07
    void(*no_function_08)(void*);                                           //0x08
    void(*no_function_09)(void*);                                           //0x09
    void(*no_function_0a)(void*);                                           //0x0a
    void(*no_function_0b)(void*);                                           //0x0b
    void(*no_function_0c)(void*);                                           //0x0c
    void(*no_function_0d)(void*);                                           //0x0d
    void(*no_function_0e)(void*);                                           //0x0e
    int (*write_bits)(modbus_t *, int , int , const uint8_t *);             //0x0f
    int (*write_registers)(modbus_t *, int , int , const uint16_t *);       //0x10
    int (*report_slave_id)(modbus_t *, int , uint8_t *);                    //0x11
    void(*no_function_12)(void*);                                           //0x12
    void(*no_function_13)(void*);                                           //0x13
    void(*no_function_14)(void*);                                           //0x14
    void(*no_function_15)(void*);                                           //0x15
    int (*mask_write_register)(modbus_t *, int , uint16_t , uint16_t );     //0x16
    int (*write_and_read_registers)(modbus_t *, int , int ,
                                               const uint16_t *, int , int ,
                                               uint16_t *);                 //0x17
}MODBUS_FUNCTION_MAP_T;
int timescale_init(void);
int timescale_create(uint32_t timeout,void *cb_param,TIMESCALE_MODE_T mode,timerfunctionCB funcb);
void * timescale_task_loop(void*args);
int timer_60s_cb(void * param);
int timer_70s_cb(void * param);
int set_script_path(char*path);
void get_luaTable_to_jsonstr( lua_State* L, int idx,char*buf);

#endif//__TIME_SCALE_H__
