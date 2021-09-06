#include "subdev.h"
#include <pthread.h>
 
#include <errno.h>
#include "unit-test.h"
#include "random-test-server.h"
#include "tuya_gw_subdev_api.h"
#include "tuya_gw_com_api.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua_modbus.h"


int isBind=0;
QUEUE_MANAGER_T *queue_dp_cmd=NULL;


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
        PR_ERR("ret=%d",ret);
    }
     PR_DEBUG("report ok");
    
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
        PR_ERR("ret=%d",ret);
    }
     PR_DEBUG("report ok");
    
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
        PR_ERR("devid=%s had register,bind_status=%d,bind=%d",dev_id,dev_if->bind_status,dev_if->bind);
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
        PR_WARN("Slave device list NULL");
        return -1;
    }
    do{
        char deviceId[64]={0};
        sprintf(deviceId,"channel_%d_slave_%d",node->channel,node->slave);
        PR_DEBUG("deviceId=%s,pid=%s",deviceId,node->pid);
        regist_subdevice(deviceId,node->pid);
        //timescale_create(node->rptInterval,node, TIMER_REPEATABLE, timer_60s_cb);
        timescale_create(node->rptInterval,node, TIMER_REPEATABLE, timer_70s_cb);
        node = node->next;
    }while(node!=NULL);
    return 0;

}


int user_save_slaveList(IN CONST CHAR_T *str_cfg)
{
    int n,ret;
    int num ;
    SLAVEINFOLIST_T *node=NULL;
    ty_cJSON *pObjCfg=NULL,*pObjSlaveArray=NULL;
    pObjCfg = ty_cJSON_Parse(str_cfg);
    if (pObjCfg == NULL) {
        PR_ERR("param cfg is invalid");
        return 0;       
    }

    pObjSlaveArray = ty_cJSON_GetObjectItem(pObjCfg, "slaves");
    if (pObjSlaveArray == NULL  || pObjSlaveArray->type != ty_cJSON_Array) {
        PR_ERR("param cfg is invalid");
        ret = -1;
        goto exit;
    }

    num = ty_cJSON_GetArraySize(pObjSlaveArray);
    if(num==0){
        PR_ERR("config.json error");
        ret = -1;
        goto exit;
    }
    for(n=0;n<num;n++){
        if((node = (SLAVEINFOLIST_T*)malloc(sizeof(SLAVEINFOLIST_T)))==NULL){
            PR_ERR("malloc failed  error");
            ret = -1;
            goto exit;
        }
        ty_cJSON*obj = ty_cJSON_GetArrayItem(pObjSlaveArray,n);
        
        ty_cJSON*objDid=ty_cJSON_GetObjectItem(obj, "did");
        if(objDid){
            strcpy(node->did,objDid->valuestring) ;
        }else{
            PR_ERR("must set device slave address");
        }
        
        ty_cJSON*objPid = ty_cJSON_GetObjectItem(obj,"pid");
        if(objPid){
            strcpy(node->pid,objPid->valuestring) ;
        }

        ty_cJSON*objDevnm=ty_cJSON_GetObjectItem(obj, "nick");
        if(objDevnm){
            strcpy(node->devName,objDevnm->valuestring) ;
        }
    
        ty_cJSON*objChannel=ty_cJSON_GetObjectItem(obj, "channel");
        if(objChannel){
            node->channel = objChannel->valueint;
        }else{
            PR_ERR("must set device channel");
        }
        
        ty_cJSON*objInterval=ty_cJSON_GetObjectItem(obj, "interval");
        if(objInterval){
            node->rptInterval = objInterval->valueint;
        }else{
            PR_ERR("must set device report interval");
        }
        
        ty_cJSON*objAddress=ty_cJSON_GetObjectItem(obj, "slave");
        if(objAddress){
            node->slave = objAddress->valueint;
        }else{
            PR_ERR("must set device slave address");
        }
        if(NULL==gSlaveDeviceInfoList){
            gSlaveDeviceInfoList = node;
            node->next = NULL;
            node->previous = NULL;
        }else{
            node->next = gSlaveDeviceInfoList;
            node->previous = NULL;
            gSlaveDeviceInfoList = node;
        }
    }
    ret=0;
exit:
    ty_cJSON_Delete(pObjCfg);
    return ret;
}

SLAVEINFOLIST_T* user_find_slaveNode_byDid(char*did){
    SLAVEINFOLIST_T*p=NULL;
    do{
        if(gSlaveDeviceInfoList==NULL){
            PR_WARN("error");
            return NULL;
        }
        p=gSlaveDeviceInfoList;
        if(strcmp(p->did,did)==0){
            return p;
        }
        p=p->next;
    }while(p!=NULL);
    return NULL;
}

void* app_main_loop(void*args){
    OPERATE_RET ret=OPRT_COM_ERROR;
    int status,val;
    TY_RECV_OBJ_DP_S*content=NULL,*p=NULL;
    DEV_DESC_IF_S*dev_if=NULL;
    SLAVEINFOLIST_T*sDevice=NULL;
     PR_DEBUG("app_main_loop pthread start");
    if((queue_dp_cmd = queue_init())==NULL){
        PR_ERR("error");
        exit(-1);
    }
    for(;;){
        sleep(1);
        do{
            ret = queue_popfront(queue_dp_cmd,(void**)&content);
            if(ret!=0){
                break;
            }
            PR_DEBUG("content=%08x",content);
            p = content;
            PR_DEBUG("p=%08x",p);


            PR_DEBUG("p=%08x",p);
            if(p->cid){
                PR_DEBUG("p->cid=%08x,%s",p->cid,p->cid);
            }

            PR_DEBUG("p->dps=%08x",p->dps);

            PR_DEBUG("p->dps[0].dpid=%d,type=%d",p->dps[0].dpid,p->dps[0].type);
            
            PR_DEBUG("p->dps[0].dpid.val=%d",p->dps[0].value);

            switch(p->dps[0].type){
            case PROP_BOOL:
                val=(int)(p->dps[0].value.dp_bool);
                break;
            case PROP_VALUE:
                val=(int)(p->dps[0].value.dp_value);
                break;
            case PROP_STR:
                //val=(int)(p->dps[0].value.dp_str;
                break;
            case PROP_ENUM:
                val=(int)(p->dps[0].value.dp_enum);
                break;
            case PROP_BITMAP:
                val=(int)(p->dps[0].value.dp_bitmap);
                break;
            default:
                break;
        }
            PR_DEBUG("33333,p->cid=%s",p->cid);
            //拿到pid
            dev_if = tuya_iot_get_dev_if(p->cid);
            if (dev_if == NULL) {
                PR_ERR("error");
                break;
            }
            PR_DEBUG("dev_if->product_key=%s,dev_if->id=%s",dev_if->product_key,dev_if->id);
            sDevice = user_find_slaveNode_byDid(dev_if->id);
            if(sDevice==NULL){
                PR_ERR("error");
                break;
            }
            //1.创建一个state
            lua_State *L = luaL_newstate();
            if (L == NULL)
            {
                PR_ERR("New state fail");
                break;
            }
            //2.加载lua库
            luaL_openlibs(L);
            //3.声明C 扩展函数
            lua_register(L, "modbus_write_bit",__modbus_write_bit);
            lua_register(L, "modbus_write_register",__modbus_write_register);
            lua_register(L, "modbus_write_bits",__modbus_write_bits);
            lua_register(L, "modbus_write_registers",__modbus_write_bits);
            lua_register(L, "modbus_report_slave_id",__modbus_report_slave_id);
            lua_register(L, "modbus_mask_write_register",__modbus_mask_write_register);
            lua_register(L, "modbus_write_and_read_registers",__modbus_write_and_read_registers);
            
            //4. 运行脚本
            char path[512]={0};
            sprintf(path,"../../lua/%s.lua",dev_if->product_key);
            int error=luaL_dofile(L, path);
            if(error) {
                PR_ERR("Error: %s", lua_tostring(L,-1));
                break;
            }
            int m = lua_gettop(L);
            //5.获得lua函数名并执行
            lua_getglobal(L,"attr");
            if(!lua_istable(L,-1)){
                PR_ERR("stack top is not table,pls check variable is not global ?");
                break;
            }
            m = lua_gettop(L);
            PR_DEBUG("stack param count=%d",m);
        
        //    printf_Lua_topTable(L);
            char buff[1024]={0};
            get_luaTable_to_jsonstr(L,1,buff);
        
            printf("%s\r\n",buff);
            ty_cJSON*obj_body = ty_cJSON_Parse(buff);
            if(obj_body ==NULL){
                PR_DEBUG("error");
                break;
            }
            ty_cJSON*objItem=NULL,*objFc=NULL,*objAddr=NULL,*objDpid=NULL,*objType=NULL;
            int i=1;
            do{
                char bufferi[2]={0};
                sprintf(bufferi,"%c",(i+0x30));
                PR_DEBUG("bufferi=%s",bufferi);
                objItem=ty_cJSON_GetObjectItem(obj_body,bufferi);
                i++;
                if(objItem){
                    objFc=ty_cJSON_GetObjectItem(objItem, "fc");
                    objAddr=ty_cJSON_GetObjectItem(objItem, "addr");
                    objDpid=ty_cJSON_GetObjectItem(objItem, "dpid");
                    objAddr=ty_cJSON_GetObjectItem(objItem, "type");
                    if((objDpid->valueint==p->dps[0].dpid)&&(objType->valueint==p->dps[0].type)){
                        if(objFc){
                            switch(objFc->valueint){
                                case 0x06:
                                {
                                    app_modbus_write_register(sDevice->did,objAddr->valueint,val,sDevice->slave,sDevice->channel);
                                    break;
                                }
                                default:
                                break;
                            }
                        }
                    }
                }
                
            }while(objItem!=NULL);


            /* 清除Lua */    
            lua_close(L); 
            for(int i=0;i<p->dps_cnt;i++){
                if(p->dps[i].type==PROP_STR){
                    free(p->dps[i].value.dp_str);
                }
            }
            free(p);
        }while(ret==0);
    }
}
