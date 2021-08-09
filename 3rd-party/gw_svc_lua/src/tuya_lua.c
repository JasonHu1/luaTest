

#include "tuya_lua.h"



STATIC LUA_ADAPTER_S lua_adapter_api;

STATIC LUA_ADAPTER_S *_get_lua_adapter(VOID)
{
    return &lua_adapter_api;
}

STATIC LUA_ADAPTER_LIST_S *_lua_add_adapter_pro(TY_LUA_SUPPORT_PROTOCOL pro, TY_LUA_ADAPTER_S *pro_api)
{

    LUA_ADAPTER_S *lua_adapter_api_tmp = _get_lua_adapter();
    LUA_ADAPTER_LIST_S *list_tmp = (LUA_ADAPTER_LIST_S *)Malloc(SIZEOF(LUA_ADAPTER_LIST_S));
    if(NULL == list_tmp){
        PR_ERR("Malloc error.");
        return NULL;
    }

    memset(list_tmp, 0, SIZEOF(LUA_ADAPTER_LIST_S));
    list_tmp->lua_pro = pro;
    
    memcpy(&list_tmp->lua_adapter_api, pro_api, SIZEOF(TY_LUA_ADAPTER_S));


    list_tmp->next = lua_adapter_api_tmp->lua_adapter_list;
    lua_adapter_api_tmp->lua_adapter_list = list_tmp;

    lua_adapter_api_tmp->list_num++;
    
    PR_DEBUG("Lua add new pro = %d, list_num:%d.",  pro, lua_adapter_api_tmp->list_num);
    
    return list_tmp;
}

STATIC LUA_ADAPTER_LIST_S *_lua_get_adapter_pro(TY_LUA_SUPPORT_PROTOCOL pro)
{

    LUA_ADAPTER_S *lua_adapter_api_tmp = _get_lua_adapter();
    LUA_ADAPTER_LIST_S *list = lua_adapter_api_tmp->lua_adapter_list;
    while(list) {
        if(list->lua_pro == pro) {
            return list;
        }

        list = list->next;
    }

    return NULL;
}

STATIC VOID __attribute__((unused)) _lua_del_adapter_pro(TY_LUA_SUPPORT_PROTOCOL pro)
{
    LUA_ADAPTER_S *lua_adapter_api_tmp = _get_lua_adapter();
    LUA_ADAPTER_LIST_S *list = lua_adapter_api_tmp->lua_adapter_list;
    LUA_ADAPTER_LIST_S *pre_list = NULL;

    while(list)
    {
        if(list->lua_pro == pro){
            lua_adapter_api_tmp->list_num--;
            if(list == lua_adapter_api_tmp->lua_adapter_list){
                lua_adapter_api_tmp->lua_adapter_list = list->next;
            }else{
                pre_list->next = list->next;
            }

            list->next = NULL;

            Free(list);
            list = NULL;

            return;
        }
        pre_list = list;
        list = list->next;
    }

    return;
}

STATIC OPERATE_RET _lua_parase_dp_down_info(IN lua_State *L, IN INT_T dp_id, OUT TY_LUA_DP_INFO_S *dp_info)
{
    CHAR_T *out_val = NULL;
    OPERATE_RET op_ret = OPRT_OK;
    CHAR_T key_str_tmp[KEY_LEN_MAX+8] = {0};
    
    snprintf(key_str_tmp, KEY_LEN_MAX, "%s%d", DO_DOWN_KEY, dp_id);
    PR_DEBUG("key_str_tmp:%s, dp_id:%d.", key_str_tmp, dp_id);
    op_ret = lua_com_get_dp_info(L, key_str_tmp, &out_val);
    
    lua_com_dp_down_info_json_parase(out_val, dp_info);

    Free(out_val);
    
    return op_ret;
}

STATIC OPERATE_RET _lua_dp_down_parase(IN lua_State *L, IN INT_T dp_id, IN TY_OBJ_DP_S *dp_obj,  TY_LUA_DP_INFO_S *dp_info, OUT CHAR_T **out_val,  OUT INT_T *len)
{
    OPERATE_RET op_ret = OPRT_OK;
    
    op_ret = _lua_parase_dp_down_info(L, dp_id, dp_info);
    if(OPRT_OK != op_ret){
        PR_ERR("lua_com_init error.");
        return op_ret;
    }
    
    op_ret = lua_com_dp_down_parseData(L, dp_info->fun, dp_obj, out_val, len);
    if(OPRT_OK != op_ret){
        PR_ERR("lua_com_init error.");
        return op_ret;
    }
    
    return op_ret;
}

STATIC OPERATE_RET _lua_dp_down(lua_State *L, IN TY_LUA_SUPPORT_PROTOCOL pro, IN TY_CMD_U *dp, IN CHAR_T type)
{
    OPERATE_RET op_ret = OPRT_OK;
    LUA_ADAPTER_LIST_S *adapter_pro = NULL;
    INT_T i = 0;
    CHAR_T *out_val = NULL;
    INT_T len = 0;
    TY_OBJ_DP_S *dp_obj = NULL;
    TY_LUA_DP_INFO_S dp_info = {0};
    
    adapter_pro = tuya_lua_get_register_pro(pro);
    if(NULL == adapter_pro){
        PR_ERR("Don't support lua protocol:%d",pro);
        return OPRT_COM_ERROR;
    }
    
    PR_DEBUG("dp->obj_dp->dps_cnt:%d",dp->obj_dp->dps_cnt);
        
    for(i = 0; i < dp->obj_dp->dps_cnt; i++){

        dp_obj = &(dp->obj_dp->dps[i]);
    
        op_ret = _lua_dp_down_parase(L, dp_obj->dpid, dp_obj, &dp_info, &out_val, &len);
        if(OPRT_OK != op_ret){
            PR_ERR("lua_com_init error.");
            continue;
        }
        if(adapter_pro->lua_adapter_api.dp_down)
            adapter_pro->lua_adapter_api.dp_down(dp->obj_dp->cid, dp_info.pro_info, out_val, len);
        
        if(out_val){
            Free(out_val);
            out_val = NULL;
        }
        len = 0;
        
        if(dp_info.pro_info){
            Free(dp_info.pro_info);
            dp_info.pro_info = NULL;
        }
    }

    return op_ret;
}



STATIC OPERATE_RET _lua_parase_dp_up_info(IN lua_State *L, CHAR_T *key_str, OUT TY_LUA_DP_INFO_S *dp_info)
{
    CHAR_T key_str_tmp[KEY_LEN_MAX+8] = {0};
    OPERATE_RET op_ret = OPRT_OK;
    CHAR_T *out_val = NULL;
    
    snprintf(key_str_tmp, KEY_LEN_MAX, "%s%s", DO_UP_KEY, key_str);
    PR_DEBUG("key_str_tmp:%s.", key_str_tmp);
    op_ret = lua_com_get_dp_info(L, key_str_tmp, &out_val);
    if(OPRT_OK != op_ret){
        PR_ERR("lua_com_get_dp_info error.");
        return op_ret;
    }
    
    op_ret = lua_com_dp_up_info_json_parase(out_val, dp_info);
    if(OPRT_OK != op_ret){
        PR_ERR("lua_com_get_dp_info error.");
    }

    if(NULL != out_val)
        Free(out_val);
    
    return op_ret;
}

STATIC OPERATE_RET _lua_dp_up(IN lua_State *L, CHAR_T *key_str, IN CHAR_T *data,  IN INT_T len, OUT TY_OBJ_DP_S **dp_data, OUT INT_T *dp_num)
{
    OPERATE_RET op_ret = OPRT_OK;
    TY_LUA_DP_INFO_S dp_info = {0};

    op_ret = _lua_parase_dp_up_info(L, key_str, &dp_info);
    if(OPRT_OK != op_ret){
        PR_ERR("_lua_parase_dp_up_info error.");
        return op_ret;
    }

    op_ret = lua_dp_up_table_parseData(L, dp_info.fun, key_str, data,  len, dp_data, dp_num);
    if(OPRT_OK != op_ret){
        PR_ERR("_lua_parase_dp_up_info error.");
        return op_ret;
    }
    
    if(NULL != dp_info.pro_info){//up的时候没用，默认是没有的
        Free(dp_info.pro_info);
    }
    
    return op_ret;
}

OPERATE_RET tuya_lua_dp_down(IN CHAR_T *lua_file, IN TY_LUA_SUPPORT_PROTOCOL pro, IN TY_CMD_U *cmd, IN CHAR_T type)
{
    OPERATE_RET op_ret = OPRT_OK;
    lua_State *L = NULL;

    op_ret = lua_com_init(lua_file, &L);
    if(OPRT_OK != op_ret){
        PR_ERR("lua_com_init error.");
        return op_ret;
    }

    op_ret =  _lua_dp_down(L, pro, cmd, type);
    if(OPRT_OK != op_ret){
        PR_ERR("_lua_dp_down error.");
        return op_ret;
    }

    lua_close(L);

    return op_ret;
}

OPERATE_RET tuya_lua_dp_up(IN CHAR_T *lua_file, IN CHAR_T *key_str, IN CHAR_T *data,  IN INT_T len, OUT TY_OBJ_DP_S **dp_data, OUT INT_T *dp_num)
{
    OPERATE_RET op_ret = OPRT_OK;
    lua_State *L = NULL;

    op_ret = lua_com_init(lua_file, &L);
    if(OPRT_OK != op_ret){
        PR_ERR("lua_com_init error.");
        return op_ret;
    }

    op_ret = _lua_dp_up(L, key_str, data, len, dp_data, dp_num);
    if(OPRT_OK != op_ret){
        PR_ERR("_lua_dp_up error.");
        return op_ret;
    }
        
    lua_close(L);

    return op_ret;
}

OPERATE_RET tuya_lua_register_pro(TY_LUA_SUPPORT_PROTOCOL pro, TY_LUA_ADAPTER_S *pro_api)
{
    LUA_ADAPTER_LIST_S *adapter_pro = NULL;
    OPERATE_RET op_ret = OPRT_OK;
    LUA_ADAPTER_S *lua_adapter_api_tmp = _get_lua_adapter();
    if(NULL == pro_api){
        PR_ERR("Param is null.");
        return OPRT_INVALID_PARM;
    }

    if(NULL == lua_adapter_api_tmp->mutex){
        op_ret = tuya_hal_mutex_create_init(&(lua_adapter_api_tmp->mutex));
        if(OPRT_OK != op_ret) {
            PR_ERR("tuya_hal_mutex_create_init err:%d",op_ret);
        }
    }

    if(lua_adapter_api_tmp->mutex)
        tuya_hal_mutex_lock(lua_adapter_api_tmp->mutex);
        
    adapter_pro = _lua_get_adapter_pro(pro);
    if(adapter_pro){
        PR_WARN("register again,pro:%d.", pro);
        memcpy(&adapter_pro->lua_adapter_api, pro_api, SIZEOF(TY_LUA_ADAPTER_S));
    }else{
        adapter_pro = _lua_add_adapter_pro(pro, pro_api);
    }

    if(adapter_pro){
        PR_DEBUG("register pro:%d success.", pro);
    }else{
        PR_ERR("register pro:%d fail.", pro);
    }
    
    if(lua_adapter_api_tmp->mutex)
        tuya_hal_mutex_unlock(lua_adapter_api_tmp->mutex);
    
    return OPRT_OK;
}


LUA_ADAPTER_LIST_S *tuya_lua_get_register_pro(TY_LUA_SUPPORT_PROTOCOL pro)
{
    LUA_ADAPTER_LIST_S *adapter_pro = NULL;
    OPERATE_RET op_ret = OPRT_OK;
    LUA_ADAPTER_S *lua_adapter_api_tmp = _get_lua_adapter();


    if(lua_adapter_api_tmp->mutex)
        tuya_hal_mutex_lock(lua_adapter_api_tmp->mutex);
        
    adapter_pro = _lua_get_adapter_pro(pro);

    if(lua_adapter_api_tmp->mutex)
        tuya_hal_mutex_unlock(lua_adapter_api_tmp->mutex);

    return adapter_pro;
}

/*
    获取协议私有数据，像zigbee，就是config report table
*/
OPERATE_RET tuya_lua_get_pro_data(IN CHAR_T *lua_file, OUT CHAR_T **pro_data_json_str)
{
    OPERATE_RET op_ret = OPRT_OK;
    lua_State *L = NULL;

    op_ret = lua_com_init(lua_file, &L);
    if(OPRT_OK != op_ret){
        PR_ERR("lua_com_init error.");
        return op_ret;
    }

    op_ret =  lua_get_pro_data(L, pro_data_json_str);
    if(OPRT_OK != op_ret){
        PR_ERR("_lua_dp_up error.");
        return op_ret;
    }
        
    lua_close(L);

    return op_ret;
}

