#include "subdev.h"
#include <pthread.h>
#include "app_defs_types.h"
#include <errno.h>
#include "unit-test.h"
#include "random-test-server.h"
#include "tuya_gw_subdev_api.h"
#include "tuya_gw_com_api.h"

int isBind=0;

//dev_report_dp_stat_sync
//dev_report_dp_json_async

int report_status(char status)
{
    OPERATE_RET ret= OPRT_OK;
    TY_OBJ_DP_S  dp_data[3];
    //doorcontact_state
    dp_data[0].dpid = 1;
    dp_data[0].type = PROP_BOOL;
    dp_data[0].value.dp_bool = status;

    //battery_percentage
    dp_data[1].dpid = 2;
    dp_data[1].type = PROP_VALUE;
    dp_data[1].value.dp_value = 33;

    //temper_alarm
    dp_data[2].dpid = 4;
    dp_data[2].type = PROP_BOOL;
    dp_data[2].value.dp_bool = status;
    if(ret = dev_report_dp_json_async(DEVICE_ID_CONTACT, (const TY_OBJ_DP_S *)&dp_data, 3)!=OPRT_OK){
        vDBG_ERR("ret=%d",ret);
    }
    vDBG_INFO("report ok");
    
}

int report_batteryVal(INT_T btVal)
{
    OPERATE_RET ret= OPRT_OK;
    TY_OBJ_DP_S  dp_data[3];

    //battery_percentage
    dp_data[0].dpid = 2;
    dp_data[0].type = PROP_VALUE;
    dp_data[0].value.dp_value = btVal;

    if(ret = dev_report_dp_json_async(DEVICE_ID_CONTACT, (const TY_OBJ_DP_S *)&dp_data, 3)!=OPRT_OK){
        vDBG_ERR("ret=%d",ret);
    }
    vDBG_INFO("report ok");
    
}

void readSerial(void){

}
void writeSeial(void){

}

int regist_subdevice(CHAR_T *dev_id,CHAR_T *pid){
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T uddd = 0x00000001;
    //CHAR_T *dev_id = DEVICE_ID_CONTACT;
    //CHAR_T *pid = PRODUCT_ID_CONTACT;
    CHAR_T *ver = "1.0.0";
    
    DEV_DESC_IF_S *dev_if = NULL;

    dev_if = tuya_iot_get_dev_if(dev_id);
    if (dev_if != NULL) {
        vDBG_ERR("devid=%s had register,bind_status=%d,bind=%d",dev_id,dev_if->bind_status,dev_if->bind);
        return -1;
    }

    op_ret = tuya_iot_gw_bind_dev(GP_DEV_ATH_1, uddd, dev_id, pid, ver);
    if (op_ret != OPRT_OK) {
        PR_WARN("tuya_iot_gw_bind_dev err: %d", op_ret);
        return -1;
    }

    return 0;
}

uint8_t buffer[1024]={0};
int timer_60s_cb(void * param);
extern SLAVEINFOLIST_T *gSlaveDeviceInfoList;

int start_slave_timeScale(void){
    SLAVEINFOLIST_T*node =  NULL;
    node = gSlaveDeviceInfoList;
    if(gSlaveDeviceInfoList==NULL){
        vDBG_WARN("Slave device list NULL");
        return -1;
    }
    do{
        char deviceId[64]={0};
        sprintf(deviceId,"channel_%d_slave_%d",node->channel,node->slave);
        vDBG_APP(DBG_MSGDUMP,"deviceId=%s,pid=%s",deviceId,node->pid);
        regist_subdevice(deviceId,node->pid);
        timescale_create(node->rptInterval,node, TIMER_REPEATABLE, timer_60s_cb);
        node = node->next;
    }while(node!=NULL);
    return 0;

}
void* app_main_loop(void*args){
    OPERATE_RET ret=OPRT_OK;
    int status;
    vDBG_INFO("app_main_loop pthread start");
    for(;;){
        //timescale_create(6, NULL, TIMER_SINGLE, timer_60s_cb);
        sleep(5);
        #if 0
//        if(!isBind){
//            vDBG_INFO("subdev is Not bind success!!!");
//            continue;
//        }

        //01 (0x01) Read Coils
        
        modbus_set_slave(ctx[FD_RANK_SERIAL_START], SERVER_ID);
        if(ret = modbus_read_bits(ctx[FD_RANK_SERIAL_START],0, 10,buffer)!= -1){
            vDBG_INFO("modbus_read_bits ok1");
            report_status(status);
            status++;
            for(int i=0;i<10;i++){
                printf("%02x ",buffer[i]);
            }
            printf("\r\n");
        }else{
            vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
        }
        //03 (0x03) Read Holding Registers
        uint16 batteryVol;
        if(ret = modbus_read_registers(ctx[FD_RANK_SERIAL_START],0, 1,&batteryVol)!= -1){
            vDBG_INFO("modbus_read_registers batteryVol=%d",batteryVol);
            report_batteryVal(batteryVol);
        }else{
            vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
        }
        #endif
        
    }
}
