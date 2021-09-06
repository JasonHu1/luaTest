#ifndef __SUBDEVICE_H__
#define __SUBDEVICE_H__
#include "tuya_iot_com_api.h"
#include "modbus.h"
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "time_scale.h"
#include "tuya_cloud_com_defs.h"
#include "hal_comm.h"

#define DEVICE_ID_CONTACT   "0006dffffe67e2ac" 
#define PRODUCT_ID_CONTACT   "sxdhauza" 


#define FD_RANK_SERIAL_START    0
#define MAX_EVENTS 10



typedef struct _slaveInfoList{
    struct _slaveInfoList *next;
    struct _slaveInfoList *previous;
    uint8_t channel;//identy which port that bind
    uint8_t did[128];//use for get lua script
    uint8_t pid[128];//use for get lua script
    uint8_t devName[128];//device nick name for user change
    uint32_t rptInterval;//report time interval
    uint8_t slave;
    void *pdata;
}SLAVEINFOLIST_T;

QUEUE_MANAGER_T *queue_dp_cmd;

int report_status(char status);
int report_loop(void*args);
int regist_subdevice(CHAR_T *dev_id,CHAR_T *pid);


SLAVEINFOLIST_T* user_find_slaveNode_byDid(char * did);
int user_save_slaveList(IN CONST CHAR_T * str_cfg);

#endif//__SUBDEVICE_H__