#include "tuya_iot_com_api.h"
#include "app_debug_printf.h"
#include "modbus.h"

#define DEVICE_ID_CONTACT   "0006dffffe67e2ac" 
#define PRODUCT_ID_CONTACT   "sxdhauza" 


#define FD_RANK_SERIAL_START    0
#define MAX_EVENTS 10

extern int isBind;
extern modbus_t *ctx[MAX_EVENTS];
int isfirst;

int report_status(char status);
int report_loop(void*args);