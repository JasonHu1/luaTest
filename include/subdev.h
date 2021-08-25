#ifndef __SUBDEVICE_H__
#define __SUBDEVICE_H__
#include "tuya_iot_com_api.h"
#include "app_debug_printf.h"
#include "modbus.h"
#include "unit-test.h"
#include <pthread.h>
#include "app_defs_types.h"
#include <errno.h>
#include "time_scale.h"
#include "tuya_cloud_com_defs.h"

#define DEVICE_ID_CONTACT   "0006dffffe67e2ac" 
#define PRODUCT_ID_CONTACT   "sxdhauza" 


#define FD_RANK_SERIAL_START    0
#define MAX_EVENTS 10

extern int isBind;
extern modbus_t *ctx[MAX_EVENTS];

int report_status(char status);
int report_loop(void*args);
int regist_subdevice(CHAR_T *dev_id,CHAR_T *pid);

#endif//__SUBDEVICE_H__