#include "tuya_lua_api.h"
#include "gw_com_def.h"

TY_LUA_ADAPTER_S *tuya_lua_register_pro_api(IN TY_LUA_SUPPORT_PROTOCOL pro, IN TY_LUA_ADAPTER_S *pro_api)
{
    return tuya_lua_register_pro(pro, pro_api);
}

OPERATE_RET tuya_lua_dp_down_api(IN CHAR_T *dev_id, IN TY_LUA_SUPPORT_PROTOCOL pro, IN TY_CMD_U *cmd, IN CHAR_T type)
{
    DEV_DESC_IF_S *dev_if = NULL;
    OPERATE_RET ret = OPRT_OK;

    dev_if = tuya_iot_get_dev_if(dev_id);
    if(NULL == dev_if){
        PR_ERR("Don't found device dev_id:%s",dev_id);
        return OPRT_COM_ERROR;
    }
    
    return tuya_lua_dp_down("/tmp/test.lua", pro, cmd, type);
}

OPERATE_RET tuya_lua_dp_up_api(IN CHAR_T *dev_id, IN CHAR_T *key_str, IN CHAR_T *data,  IN INT_T len, OUT TY_OBJ_DP_S **dp_data, OUT INT_T *dp_num)
{
    DEV_DESC_IF_S *dev_if = NULL;
    OPERATE_RET ret = OPRT_OK;

    dev_if = tuya_iot_get_dev_if(dev_id);
    if(NULL == dev_if){
        PR_ERR("Don't found device dev_id:%s",dev_id);
        return OPRT_COM_ERROR;
    }
    
    //dev_if->lua_file
    return tuya_lua_dp_up("/tmp/test.lua", key_str, data, len, dp_data, dp_num);
}


OPERATE_RET tuya_lua_get_pro_data_api(IN CHAR_T *dev_id, OUT CHAR_T **pro_data_json_str)
{
    DEV_DESC_IF_S *dev_if = NULL;
    OPERATE_RET ret = OPRT_OK;

    dev_if = tuya_iot_get_dev_if(dev_id);
    if(NULL == dev_if){
        PR_ERR("Don't found device dev_id:%s",dev_id);
        return OPRT_COM_ERROR;
    }
    
    return tuya_lua_get_pro_data("/tmp/test.lua", pro_data_json_str);
}

