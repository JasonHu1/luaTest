#include "subdev.h"
#include <pthread.h>
#include "app_defs_types.h"
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
        vDBG_ERR("config.json error");
        ret = -1;
        goto exit;
    }
    for(n=0;n<num;n++){
        if((node = (SLAVEINFOLIST_T*)malloc(sizeof(SLAVEINFOLIST_T)))==NULL){
            vDBG_ERR("malloc failed  error");
            ret = -1;
            goto exit;
        }
        ty_cJSON*obj = ty_cJSON_GetArrayItem(pObjSlaveArray,n);
        
        ty_cJSON*objDid=ty_cJSON_GetObjectItem(obj, "did");
        if(objDid){
            strcpy(node->did,objDid->valuestring) ;
        }else{
            vDBG_ERR("must set device slave address");
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
            vDBG_ERR("must set device channel");
        }
        
        ty_cJSON*objInterval=ty_cJSON_GetObjectItem(obj, "interval");
        if(objInterval){
            node->rptInterval = objInterval->valueint;
        }else{
            vDBG_ERR("must set device report interval");
        }
        
        ty_cJSON*objAddress=ty_cJSON_GetObjectItem(obj, "slave");
        if(objAddress){
            node->slave = objAddress->valueint;
        }else{
            vDBG_ERR("must set device slave address");
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
            vDBG_DEBUG("error");
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
    int status;
    TY_RECV_OBJ_DP_S*content=NULL,*p=NULL;
    DEV_DESC_IF_S*dev_if=NULL;
    SLAVEINFOLIST_T*sDevice=NULL;
    vDBG_INFO("app_main_loop pthread start");
    if((queue_dp_cmd = queue_init())==NULL){
        vDBG_ERR("error");
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

            
            vDBG_APP(DBG_DEBUG,"33333,p->cid=%s",p->cid);
            //拿到pid
            dev_if = tuya_iot_get_dev_if(p->cid);
            if (dev_if == NULL) {
                vDBG_ERR("error");
                break;
            }
            vDBG_APP(DBG_DEBUG,"dev_if->product_key=%s,dev_if->id=%s",dev_if->product_key,dev_if->id);
            sDevice = user_find_slaveNode_byDid(dev_if->id);
            if(sDevice==NULL){
                vDBG_ERR("error");
                break;
            }
            //1.创建一个state
            lua_State *L = luaL_newstate();
            if (L == NULL)
            {
                vDBG_ERR("New state fail");
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
                vDBG_ERR("Error: %s", lua_tostring(L,-1));
                break;
            }

            //5.获得lua函数名并执行
            lua_getglobal(L,"write_dp");

            char deviceId[64]={0};
            sprintf(deviceId,"channel_%d_slave_%d",sDevice->channel,sDevice->slave);

            /*6.参数入栈*/
            lua_pushinteger(L, sDevice->slave);   // 压入第一个参数  
            lua_pushstring(L, deviceId);          // 压入第二个参数
            lua_pushinteger(L, sDevice->channel);
            
            lua_newtable(L);
            lua_pushinteger(L,1);
            for(int i=1;i<p->dps_cnt+1;i++){
                lua_newtable(L);
                lua_pushstring(L, "dpid");
                lua_pushinteger(L,p->dps[i-1].dpid);
                lua_settable(L,-3);
                lua_pushstring(L, "val");
                switch (p->dps[i-1].type) {
                    case PROP_BOOL:
                        lua_pushinteger(L,p->dps[i-1].value.dp_bool);
                        break;
                    case PROP_VALUE:
                    PR_DEBUG("p->dps[i-1].value.dp_value=%d",p->dps[i-1].value.dp_value);
                        lua_pushinteger(L,p->dps[i-1].value.dp_value);
                        break;
                    case PROP_ENUM:
                        lua_pushinteger(L,p->dps[i-1].value.dp_enum);
                        break;
                    case PROP_STR:
                        lua_pushstring(L,p->dps[i-1].value.dp_str);
                        break;
                }
                lua_settable(L,-3);
            }
            lua_settable(L,-3);

            //7.执行指定lua函数
            if((ret = lua_pcall(L, 4, 0, 0))!=LUA_OK)//有4个入参数，0个返回值
            {
                const char *pErrorMsg = lua_tostring(L, -1);  
                vDBG_ERR("ret=%d,%s",ret,pErrorMsg);
                lua_close(L);  
                break; 
            }

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
