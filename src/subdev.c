#include "subdev.h"
#include <pthread.h>
#include "app_defs_types.h"
#include <errno.h>
#include "unit-test.h"

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


uint8_t buffer[1024]={0};
int timer_60s_cb(void * param);

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
