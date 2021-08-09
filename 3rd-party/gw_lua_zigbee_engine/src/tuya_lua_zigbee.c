/*
    zigbee解析引擎
*/

//#include "tuya_lua_zigbee_engine.h"
#include "tuya_lua_zigbee.h"
#include "tuya_z3_base.h"
#include "tuya_iot_com_api.h"


STATIC OPERATE_RET _lua_dp_down_pro_data(CHAR_T *dp_info_str, OUT TY_LUA_ZE_DOWN_DP_INFO *dp_info)
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *dp_info_json = NULL;
    ty_cJSON *child_json = NULL;

    dp_info_json = ty_cJSON_Parse(dp_info_str);
    if(NULL == dp_info_json){
        PR_ERR("Json is null.");
        return OPRT_COM_ERROR;
    }
   
    child_json = ty_cJSON_GetObjectItem(dp_info_json, EP_KYE);
    if(NULL == child_json || child_json->type != ty_cJSON_Number) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s", EP_KYE);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->ep = child_json->valueint;
    
    
    child_json = ty_cJSON_GetObjectItem(dp_info_json, CLUSTER_KEY);
    if(NULL == child_json || child_json->type != ty_cJSON_Number) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",CLUSTER_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->clusterId = child_json->valueint;
#if 0
    child_json = ty_cJSON_GetObjectItem(dp_info_json, CMD_KEY);
    if(NULL == child_json || child_json->type != ty_cJSON_Number) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",CMD_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->cmd = child_json->valueint;
#endif
    child_json = ty_cJSON_GetObjectItem(dp_info_json, CMDTYPE_KEY);
    if(NULL == child_json || child_json->type != ty_cJSON_Number) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",CMDTYPE_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->cmdType = child_json->valueint;

    INFO_JSON_ERR:
        ty_cJSON_Delete(dp_info_json);

    return op_ret;
}


STATIC OPERATE_RET lua_zigbee_up(IN TuYaApsFrame *sTyApsFrameTmp, LUA_KEY_DATA_S **data_array, INT_T *arry_num)
{
    OPERATE_RET op_ret = OPRT_OK;
    TY_ATTR_UNIT_S unit_list[16] = {0};
    INT_T unit_cnt = 0;
    INT_T i = 0;
    UCHAR_T *buffer = NULL;
    USHORT_T bufLen = 0;
    UCHAR_T command = 0;
    USHORT_T clusterId =0;
    UCHAR_T  ep = 0;
    USHORT_T attr_id = 0;
    UCHAR_T  cmdType = 0;
    LUA_KEY_DATA_S *data_array_tmp = NULL;
    
    clusterId = sTyApsFrameTmp->sHeader.clusterId;
    ep = sTyApsFrameTmp->sHeader.sourceEndpoint;
    cmdType = sTyApsFrameTmp->sHeader.cmdType;
    command= sTyApsFrameTmp->sHeader.command;
    bufLen = sTyApsFrameTmp->sHeader.messageLength;
    buffer = sTyApsFrameTmp->message;

    PR_DEBUG("Cluster id:%04x.", clusterId);
    PR_DEBUG("ep:%02x.", ep);
    PR_DEBUG("cmdType:%d.", cmdType);
    PR_DEBUG("command:%02x.", command);

     
    if(TY_CMD_TYPE_GLOBAL == cmdType){
        tuya_z3_GetAttributerList(buffer, bufLen, command, unit_list, CNTSOF(unit_list), &unit_cnt);       
    }else if(TY_CMD_TYPE_PRIVATE == cmdType){
        unit_cnt = 1;
    }else{
        PR_ERR("Don't support data type:%d.", cmdType);
        return;
    }
    PR_DEBUG("unit_cnt:%d.", unit_cnt);

    data_array_tmp = (LUA_KEY_DATA_S *)Malloc(SIZEOF(LUA_KEY_DATA_S)*unit_cnt);
    if(NULL == data_array_tmp){
        PR_ERR("Malloc is error.");
        return OPRT_MALLOC_FAILED;
    }
    memset(data_array_tmp, 0, SIZEOF(LUA_KEY_DATA_S)*unit_cnt);    

    for(i = 0; i < unit_cnt; i++){//unit_cnt多个属性
        if(TY_CMD_TYPE_GLOBAL == cmdType){
            attr_id = unit_list[i].id;
            snprintf(data_array_tmp[i].key_str, KEY_LEN_MAX, "%02x_%04x_%04x", ep, clusterId, attr_id);
        }else{
            snprintf(data_array_tmp[i].key_str, KEY_LEN_MAX, "%02x_%04x_%02x", ep, clusterId, command);
        }
        data_array_tmp[i].data = unit_list[i].data;
        data_array_tmp[i].len = unit_list[i].len;
    }
    
    *arry_num = unit_cnt;
    *data_array = data_array_tmp;
    
    return OPRT_OK;
}

/*
* pro_data:zcl header的json串
* data:zcl payload序列化数据
*/
STATIC OPERATE_RET lua_zigbee_down(CHAR_T *dev_id, CHAR_T *pro_data, CHAR_T *data, INT_T len)
{
    OPERATE_RET op_ret = OPRT_OK;
    INT_T i = 0;
    INT_T ret = 0;
    TY_LUA_ZE_DOWN_DP_INFO dp_info = {0};
    TuYaApsFrame sTyApsFrameTmp = {0};
    INT_T cmd = 0;
    INT_T data_len = 0;
    
    _lua_dp_down_pro_data(pro_data, &dp_info);

    
    NODE_STRIN_TO_EUI64(dev_id, sTyApsFrameTmp.sHeader.nodeEui64);

    sTyApsFrameTmp.sHeader.profileId = PROFILE_ID_HA;
    sTyApsFrameTmp.sHeader.isNoAck = 1;
    
    sTyApsFrameTmp.sHeader.clusterId = dp_info.clusterId;
    sTyApsFrameTmp.sHeader.destinationEndpoint = dp_info.ep;
    sTyApsFrameTmp.sHeader.sourceEndpoint = 0x01;
    sTyApsFrameTmp.sHeader.cmdType = dp_info.cmdType;
    for(i = 0; i<len; i++)
    {
        PR_DEBUG("data[%d]:0x%x.", i, data[i]);
    }
    for(i = 0; i< len;){
        sTyApsFrameTmp.sHeader.command = data[i];
        data_len = data[i+1];
        if(0 != data_len){
            sTyApsFrameTmp.sHeader.messageLength = data_len;
            sTyApsFrameTmp.message = &data[i+2];
        }
        tuya_z3_ApsFrameMsgSendTo(&sTyApsFrameTmp);
        i += 2+data_len;
        
        PR_DEBUG("i :%d, len:%d.", i, len);
    }
}


#define REP_TABLE_KEY "r_table"
#define CLUSTER_ID_KEY "c_id"//cluster id
#define ATTR_ID_KEY "a_id"//attrbute id
#define MIN_KEY "min"//attrbute id
#define MAX_KEY "max"//max
#define TYPE_KEY "type"//config val type
#define VAL_KEY "val"//config val

STATIC OPERATE_RET _lua_parase_rep_table_data(CHAR_T *pro_data_json_str, TY_LUA_ZE_PRO_DATA *pro_data)
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *pro_info_json = NULL;
    ty_cJSON *rep_table_child_json = NULL;
    ty_cJSON *rep_table_json = NULL;
    ty_cJSON *child_json = NULL;
    INT_T num = 0;
    INT_T i = 0;
    TY_REP_TABLE *rep_table = NULL;
    INT_T rep_table_num = 0;
    
    pro_info_json = ty_cJSON_Parse(pro_data_json_str);
    if(NULL == pro_info_json){
        PR_ERR("Json is null.");
        return OPRT_COM_ERROR;
    }
   
    rep_table_json = ty_cJSON_GetObjectItem(pro_info_json, REP_TABLE_KEY);
    if(NULL == rep_table_json || rep_table_json->type != ty_cJSON_Array) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s", REP_TABLE_KEY);
        ty_cJSON_Delete(pro_info_json);
        return OPRT_CJSON_GET_ERR;
    }
    
    num = ty_cJSON_GetArraySize(rep_table_json);
    PR_DEBUG("report table num:%d.", num);
    rep_table_num = num;
    rep_table = (TY_REP_TABLE *)Malloc(SIZEOF(TY_REP_TABLE)*rep_table_num);
    if(NULL == rep_table){
        PR_ERR("re[_tab;e is null.");
        ty_cJSON_Delete(pro_info_json);
        return OPRT_COM_ERROR;
    }
    
    for(i = 0; i < num; i++) {
        rep_table_child_json = ty_cJSON_GetArrayItem(rep_table_json, i);
        if(NULL == rep_table_child_json) {
            PR_ERR("ty_cJSON_GetArrayItem error.");
            op_ret = OPRT_CJSON_GET_ERR;
            goto INFO_JSON_ERR;
        }

        child_json = ty_cJSON_GetObjectItem(rep_table_child_json, CLUSTER_ID_KEY);//cluster id
        if(NULL == child_json) {
            PR_ERR("ty_cJSON_GetObjectItem error,str:%s.", CLUSTER_ID_KEY);
            op_ret = OPRT_CJSON_GET_ERR;
            goto INFO_JSON_ERR;
        }
        rep_table[i].c_id = child_json->valueint;
        
        child_json = ty_cJSON_GetObjectItem(rep_table_child_json, ATTR_ID_KEY);
        if(NULL == child_json) {
            PR_ERR("ty_cJSON_GetObjectItem error,str:%s.", ATTR_ID_KEY);
            op_ret = OPRT_CJSON_GET_ERR;
            goto INFO_JSON_ERR;
        }
        rep_table[i].a_id = child_json->valueint;

        child_json = ty_cJSON_GetObjectItem(rep_table_child_json, MIN_KEY);
        if(NULL == child_json) {
            PR_ERR("ty_cJSON_GetObjectItem error,str:%s.", MIN_KEY);
            op_ret = OPRT_CJSON_GET_ERR;
            goto INFO_JSON_ERR;
        }
        rep_table[i].min = child_json->valueint;

        child_json = ty_cJSON_GetObjectItem(rep_table_child_json, MAX_KEY);
        if(NULL == child_json) {
            PR_ERR("ty_cJSON_GetObjectItem error,str:%s.", MAX_KEY);
            op_ret = OPRT_CJSON_GET_ERR;
            goto INFO_JSON_ERR;
        }
        rep_table[i].max = child_json->valueint;

        child_json = ty_cJSON_GetObjectItem(rep_table_child_json, TYPE_KEY);
        if(NULL == child_json) {
            PR_ERR("ty_cJSON_GetObjectItem error,str:%s.", TYPE_KEY);
            op_ret = OPRT_CJSON_GET_ERR;
            goto INFO_JSON_ERR;
        }
        rep_table[i].type = child_json->valueint;

        child_json = ty_cJSON_GetObjectItem(rep_table_child_json, VAL_KEY);
        if(NULL == child_json) {
            PR_ERR("ty_cJSON_GetObjectItem error,str:%s.", VAL_KEY);
            op_ret = OPRT_CJSON_GET_ERR;
            goto INFO_JSON_ERR;
        }
        rep_table[i].val = child_json->valueint;
        PR_DEBUG("rep_table[%d]->c_id: 0x%x.", i, rep_table[i].c_id);
        PR_DEBUG("rep_table[%d]->a_id: 0x%x.", i, rep_table[i].a_id);
        PR_DEBUG("rep_table[%d]->min: %u.", i, rep_table[i].min);
        PR_DEBUG("rep_table[%d]->max: %u.", i, rep_table[i].max);
        PR_DEBUG("rep_table[%d]->type: %d.", i, rep_table[i].type);
        PR_DEBUG("rep_table[%d]->val: %d.", i, rep_table[i].val);
    }
    pro_data->rep_table = rep_table;
    pro_data->rep_table_num = rep_table_num;
    
    INFO_JSON_ERR:
        ty_cJSON_Delete(pro_info_json);
        Free(rep_table);
    return op_ret;
}

STATIC OPERATE_RET __attribute__((unused)) tuya_zigbee_lua_pro_data(CHAR_T *dev_id, TY_LUA_ZE_PRO_DATA *pro_data)
{
    CHAR_T *pro_data_json_str = NULL;
    OPERATE_RET ret = OPRT_OK;
    ret = tuya_lua_get_pro_data_api(dev_id, &pro_data_json_str);
    if(OPRT_OK != ret){
        PR_ERR("tuya_lua_get_pro_data_api error.");
        return OPRT_COM_ERROR;
    }

    ret = _lua_parase_rep_table_data(pro_data_json_str, pro_data);
    if(OPRT_OK != ret){
        PR_ERR("_lua_parase_rep_table_data error.");
    }
    
    if(NULL != pro_data_json_str)
        Free(pro_data_json_str);
        
}


OPERATE_RET tuya_zigbee_lua_report(IN TuYaApsFrame *sTyApsFrameTmp)
{
    OPERATE_RET ret = OPRT_OK;
    CHAR_T dev_id[DEV_ID_MAX_LEN+1] = {0};
    TY_OBJ_DP_S *dp_data = NULL;
    INT_T dp_num = 0; 
    LUA_KEY_DATA_S *data_array = NULL;
    INT_T arry_num = 0;
    INT_T i = 0;
    
    NODE_EUI64_TO_STRIN(dev_id, sTyApsFrameTmp->sHeader.nodeEui64);

    //从上报的pro数据里，获取查找的key及需传入的raw数据
    lua_zigbee_up(sTyApsFrameTmp, &data_array, &arry_num);

    for(i = 0; i < arry_num; i++){
        ret = tuya_lua_dp_up_api(dev_id, data_array[i].key_str, data_array[i].data, data_array[i].len, &dp_data, &dp_num);
        PR_DEBUG("********************************************up_test_end********************************************");
        dev_report_dp_json_async(dev_id, dp_data, dp_num);
        Free(dp_data);
        dp_data = NULL;
        dp_num = 0;
    }
    
    Free(data_array);

    return ret;
}

VOID tuya_lua_zigbee_init(VOID)
{
    TY_LUA_ADAPTER_S lua_api_s = {
        .dp_down = lua_zigbee_down,
    };
    
    tuya_lua_register_pro_api(TY_LUA_ZIGBEE, &lua_api_s);
}
