#include "front_ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tuya_iot_com_api.h"
#include "tuya_iot_sdk_api.h"
#include "tuya_iot_sdk_defs.h"
#include "uni_log.h"
#include "tuya_gw_subdev_api.h"
extern CHAR_T *gCfg_str;

void MsgHandler(void *msgData)
{
    ipc_buff sMsg,*msgBuff;
    msgBuff = (ipc_buff *)msgData;
    printf("Receive From Server: Type:[%d], Data:[\"%s\"]\n\n",
           msgBuff->msg_type, msgBuff->msg_data);
    switch(msgBuff->msg_type){
        case GATEWAY_UART_INFO:
        {
            ty_cJSON *pObjCfg=NULL,*pObjUartArray=NULL;
            if(gCfg_str==NULL){
                PR_ERR("gCfg_str is null");
                return;
            }
            pObjCfg = ty_cJSON_Parse(gCfg_str);
            if (pObjCfg == NULL) {
                PR_ERR("param cfg is invalid");
                return;
            }

            pObjUartArray = ty_cJSON_GetObjectItem(pObjCfg, "uart");
            if (pObjUartArray == NULL  || pObjUartArray->type != ty_cJSON_Array) {
                PR_ERR("param cfg is invalid");
                return;
            }
            
            sMsg.msg_type = GATEWAY_UART_INFO;
            char *jsonStr=ty_cJSON_PrintUnformatted(pObjUartArray);
            strcpy(sMsg.msg_data,jsonStr);
            ipc_send((void*)&sMsg);
            free(jsonStr);
            break;
        }
        case GATEWAY_UART_CONFIG:
        break;
        case DEV_DRIVE_INFO:
        {
            ty_cJSON *pObjCfg=NULL,*pObjSlavesArray=NULL;
            if(gCfg_str==NULL){
                PR_ERR("gCfg_str is null");
                return;
            }
            pObjCfg = ty_cJSON_Parse(gCfg_str);
            if (pObjCfg == NULL) {
                PR_ERR("param cfg is invalid");
                return;
            }

            pObjSlavesArray = ty_cJSON_GetObjectItem(pObjCfg, "slaves");
            if (pObjSlavesArray == NULL  || pObjSlavesArray->type != ty_cJSON_Array) {
                PR_ERR("param cfg is invalid");
                return;
            }
            
            sMsg.msg_type = DEV_DRIVE_INFO;
            char *jsonStr=ty_cJSON_PrintUnformatted(pObjSlavesArray);
            strcpy(sMsg.msg_data,jsonStr);
            ipc_send((void*)&sMsg);
            free(jsonStr);
            break;
        }
        case DEV_DRIVE_CONFIG:
        break;
        default:
        break;
    }
}
