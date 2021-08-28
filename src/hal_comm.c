#include "hal_comm.h"

COMM_INFO_T gConnArray[10]={0};
int gSerialNm;

COMM_INFO_T* user_get_uartConfigure(CHAR_T *cnt){
    *cnt = gSerialNm;
    return gConnArray;
}

COMM_INFO_T* user_get_conn_context_byChannel(CHAR_T channel){
    for(int i=0;i<gSerialNm;i++){
        if(gConnArray[i].pCfg->channel == channel){
            return &gConnArray[i];
        }
    }
    return NULL;;
}


/**
 * json config:
 * {
 *     "pid": <xxx>,
 *     "uuid": <...>,
 *     "authkey_key": <...>,
 *     "ap_ssid": <... optional>,
 *     "ap_password": <... optional>,
 *     "log_level": <level>,
 *     "sw_ver": <xxx>
 *     "storage_path": <...>,
 *     "start_mode": <xxxx>,
 *     "tuya": {
 *         "zigbee": {
 *                       "storage_path": <xxx, string>,
 *                       "cache_path": <xxx, string>,
 *                       "dev_name": <xxx, string>,
 *                       "cts": <xxx, number>,
 *                       "thread_mode": <xxx, number>,
 *                       "sw_ver": <xxx, string>
 *         },
 *         "bt": {
 *                   "enable_hb": <xxx, number>,
 *                   "scan_timeout": <xxx, number>,
 *                   "mode": <xxx, number>,
 *                   "sw_ver": <xxx, string>
 *         }
 *     },
 *     "user": {
 *         [ { "tp": <DEV_ATTACH_MOD_X, number>, "sw_ver": <version, string> }, ... ]
 *     },
         "uart":[
            {
                "proto":"modbus",
                "dev_nmae":"/dev/ttyUSB0",
                "baud":115200,
                "parity":"E",
                "data_bits":8,
                "stop_bits":1,
                "channel":1
            },
            {
                "proto":"modbus",
                "dev_nmae":"/dev/ttyUSB1",
                "baud":115200,
                "parity":"E",
                "data_bits":8,
                "stop_bits":1,
                "channel":1
            }
        ]
 * }
 */
int user_save_uartConfigure(IN CONST CHAR_T *str_cfg)
{
    int n,ret;
    int num ;
    COMM_INFO_T*pUartCfg=NULL;
    ty_cJSON *pObjCfg=NULL,*pObjUartArray=NULL;
    pObjCfg = ty_cJSON_Parse(str_cfg);
    if (pObjCfg == NULL) {
        PR_ERR("param cfg is invalid");
        return 0;       
    }

    pObjUartArray = ty_cJSON_GetObjectItem(pObjCfg, "uart");
    if (pObjUartArray == NULL  || pObjUartArray->type != ty_cJSON_Array) {
        PR_ERR("param cfg is invalid");
        num = 0;
        goto exit;
    }

    num = ty_cJSON_GetArraySize(pObjUartArray);
    if(num==0){
        vDBG_ERR("config.json error");
        num = 0;
        goto exit;
    }
    
    gSerialNm = num;

    for(int i=0;i<gSerialNm;i++){
        if((gConnArray[i].pCfg = (UARTCFG_T*)malloc(sizeof(UARTCFG_T)))==NULL){
            vDBG_ERR("malloc failed  error");
            num = 0;
            goto exit;
        }
        gConnArray[i].pConnCxt = NULL;
    }
    pUartCfg = gConnArray;
    for(int i=0;i<num;i++){
        ty_cJSON*obj = ty_cJSON_GetArrayItem(pObjUartArray,i);
        ty_cJSON*objP = ty_cJSON_GetObjectItem(obj,"proto");
        if(strcmp(objP->valuestring,"modbus")==0){
            pUartCfg[i].pCfg->busProto = BUS_PROTOCOL_MODBUS;
        }else{
        }
        
        ty_cJSON*objParity=ty_cJSON_GetObjectItem(obj, "parity");
        if(strcmp(objParity->valuestring,"none")==0){
            pUartCfg[i].pCfg->parity = 'N';
        }else if(strcmp(objParity->valuestring,"even")==0){
            pUartCfg[i].pCfg->parity = 'E';
        }else{
            pUartCfg[i].pCfg->parity = 'O';
        }
        
        ty_cJSON*objDevnm=ty_cJSON_GetObjectItem(obj, "dev_nmae");
        strcpy(pUartCfg[i].pCfg->devName,objDevnm->valuestring) ;

        ty_cJSON*objChannel=ty_cJSON_GetObjectItem(obj, "channel");
        pUartCfg[i].pCfg->channel = objChannel->valueint;
        
        ty_cJSON*objBaud=ty_cJSON_GetObjectItem(obj, "baud");
        pUartCfg[i].pCfg->baud = objBaud->valueint;
        
        ty_cJSON*objDatabit=ty_cJSON_GetObjectItem(obj, "data_bits");
        pUartCfg[i].pCfg->dataBit = objDatabit->valueint;
        
        ty_cJSON*objStopbit=ty_cJSON_GetObjectItem(obj, "stop_bits");
        pUartCfg[i].pCfg->stopBit = objStopbit->valueint;
    }
exit:
    ty_cJSON_Delete(pObjCfg);
    return num;
}

