
#include "lua_com_api.h"
#include "luaconf.h"

STATIC OPERATE_RET lua_get_global_key_str(IN lua_State *L, CHAR_T *key_str, CHAR_T **out_val)
{
    CHAR_T *str = NULL;
    CHAR_T *out_str = NULL;
    
    lua_getglobal(L, key_str); 
    if(LUA_TSTRING != lua_type(L,-1)){
        PR_ERR("data type error:type:%d.", lua_type(L,-1));
        return OPRT_COM_ERROR;
    }
    
    str = lua_tostring(L,-1);
    if(NULL == str){
        PR_ERR("lua_tostring is null.");
        return OPRT_COM_ERROR;
    }
    
    out_str = Malloc(strlen(str)+1);
    if(NULL == out_str){
        PR_ERR("malloc error.");
        lua_pop(L, 1);
        return OPRT_MALLOC_FAILED;
    }
    
    memset(out_str, 0, strlen(str)+1);
    strncpy(out_str, str, strlen(str));
    PR_DEBUG("out_str = %s.", out_str);
    *out_val = out_str;
    
    lua_pop(L, 1);

    return OPRT_OK;
}

STATIC OPERATE_RET lua_get_table_num_val(lua_State *L, CHAR_T *index_str, INT_T *out_val)
{
    INT_T ret = 0;
    INT_T luaType = 0;
    
    lua_getfield (L, -1, index_str);
    luaType = lua_type(L, -1);
    if(LUA_TNUMBER != luaType){
        PR_ERR("lua_getfield:%s type error:%d.\n", index_str, luaType);
    }else{
        *out_val = (int)lua_tointeger(L, -1);        
        PR_DEBUG("%s: val:%d.\n", index_str, *out_val);
    }

    lua_pop(L, 1);//值出栈，栈顶还是table

    return 0;
}

STATIC OPERATE_RET lua_get_table_bool_val(lua_State *L, CHAR_T *index_str, int *out_val)
{
    INT_T ret = 0;
    INT_T luaType = 0;
    
    lua_getfield (L, -1, index_str);

    luaType = lua_type(L, -1);
    if(LUA_TBOOLEAN != luaType){
        PR_ERR("lua_getfield:%s type error:%d.\n", index_str, luaType);
    }else{
        *out_val = (int)lua_toboolean(L, -1);
        PR_DEBUG("%s:val:%d.\n", index_str, *out_val);
    }
    
    lua_pop(L, 1);//值出栈，栈顶还是table

    return 0;
}

STATIC OPERATE_RET lua_get_table_string_val(lua_State *L, CHAR_T *index_str, CHAR_T **out_val)
{
    INT_T ret = 0;
    INT_T luaType = 0;
    CHAR_T *str_val = NULL;
    CHAR_T *out = NULL;
    INT_T len = 0;
    lua_getfield (L, -1, index_str);

    luaType = lua_type(L, -1);
    if(LUA_TSTRING != luaType){
        PR_ERR("lua_getfield:%s type error:%d.\n", index_str, luaType);
    }else{
        
        str_val = lua_tostring(L, -1);
        len = strlen(str_val);
        out = (char *)Malloc(len+1);
        if(NULL != out){
            strncpy(out, str_val, len+1);
        }
        *out_val = out;
        PR_DEBUG("%s: val:%s.\n", index_str, out);
    }
        
    lua_pop(L, 1);//值出栈，栈顶还是table

    return 0;
}

//保证栈顶是table
STATIC OPERATE_RET lua_com_table_push_val(lua_State *L, CHAR_T *index_str, INT_T val)
{
    lua_pushstring(L, index_str); //设置table的KEY
    lua_pushinteger(L, val);      //设置table的value
    lua_settable(L, -3);          //写入table 

    return OPRT_OK;
}

STATIC OPERATE_RET lua_com_table_push_bool(lua_State *L, CHAR_T *index_str, BOOL_T val)
{
    lua_pushstring(L, index_str);
    lua_pushboolean(L, val);
    lua_settable(L,-3);
    
    return OPRT_OK;
}
 
STATIC OPERATE_RET lua_com_table_push_string(lua_State *L, CHAR_T *index_str, CHAR_T *val)
{
    lua_pushstring(L, index_str);
    lua_pushstring(L, val);
    lua_settable(L,-3);
    
    return OPRT_OK;
}

STATIC VOID lua_com_get_up_dp_info(IN lua_State *L, OUT TY_OBJ_DP_S *dp_data)
{
    INT_T out_val = 0;
    INT_T dataType = 0;
    char *out_val_string = 0;

    lua_get_table_num_val(L, "dpId", &out_val);
    dp_data->dpid = out_val;
    lua_get_table_num_val(L, "type", &out_val);
    dp_data->type = out_val;
    dataType = out_val;
    lua_get_table_num_val(L, "len", &out_val);
    
    switch(dataType){
        case PROP_BOOL:
            lua_get_table_bool_val(L, "val", &out_val);
            dp_data->value.dp_bool = out_val;
            break;
        case PROP_VALUE:
        case PROP_ENUM:
        case PROP_BITMAP:
            lua_get_table_num_val(L, "val", &out_val);
            if(PROP_VALUE == dataType){
                dp_data->value.dp_value = out_val;
            }else if(PROP_ENUM == dataType){
                dp_data->value.dp_enum = out_val;
            }else{
                dp_data->value.dp_bitmap = out_val;
            }
    
            break;
        case PROP_STR:
            lua_get_table_string_val(L, "val", &out_val_string);
            PR_DEBUG("out_val_string:  %s.\n", out_val_string);
            dp_data->value.dp_str = out_val_string;
            break;
        default:
            PR_DEBUG("dataType error:%d.\n", dataType);
            break;
    }                
    lua_pop(L, 1);//把table[i]出栈
}

STATIC OPERATE_RET lua_com_get_pro_data(IN lua_State *L, OUT CHAR_T **pro_data_json_str)
{
    #define PRO_DATA_KEY "pro_data"

    return lua_get_global_key_str(L, PRO_DATA_KEY, pro_data_json_str);
}


STATIC OPERATE_RET __attribute__((unused)) lua_com_get_header_data(IN lua_State *L, OUT LUA_HEARD_INFO_S *head_info)
{
    CHAR_T *str = NULL;
    CHAR_T *out_str = NULL;
    OPERATE_RET op_ret = OPRT_OK;
    if(OPRT_OK == lua_get_global_key_str(L, "lua_ver", out_str)){
        snprintf(head_info->lua_ver, out_str, LUA_VER_LEN);
        Free(out_str);
        out_str = NULL;
    }else{
        PR_ERR("lua_get_global_key_str error.");
    }

    if(OPRT_OK == lua_get_global_key_str(L, "manufacturer_name", out_str)){
        snprintf(head_info->lua_manu_name, out_str, LUA_MANU_NAME_LEN);
        Free(out_str);
        out_str = NULL;
    }else{
        PR_ERR("lua_get_global_key_str error.");
    }
    
    if(OPRT_OK == lua_get_global_key_str(L, "model_id", out_str)){
        snprintf(head_info->lua_mode_id, out_str, LUA_MODE_ID_LEN);
        Free(out_str);
        out_str = NULL;
    }else{
        PR_ERR("lua_get_global_key_str error.");
    }

    if(OPRT_OK == lua_get_global_key_str(L, "protocol", out_str)){
        snprintf(head_info->lua_pro, out_str, LUA_PRO_LEN);
        Free(out_str);
        out_str = NULL;
    }else{
        PR_ERR("lua_get_global_key_str error.");
    }

    if(OPRT_OK == lua_get_global_key_str(L, "pid", out_str)){
        snprintf(head_info->lua_pid, out_str, LUA_PRO_LEN);
        Free(out_str);
        out_str = NULL;
    }else{
        PR_ERR("lua_get_global_key_str error.");
    }

    if(OPRT_OK == lua_get_global_key_str(L, "lua_pid_ver", out_str)){
        snprintf(head_info->lua_pid_ver, out_str, LUA_PRO_LEN);
        Free(out_str);
        out_str = NULL;
    }else{
        PR_ERR("lua_get_global_key_str error.");
    }

    return OPRT_OK;
}


OPERATE_RET lua_com_dp_down_info_json_parase(IN CHAR_T *dp_info_str, OUT TY_LUA_DP_INFO_S *dp_info)
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *dp_info_json = NULL;
    ty_cJSON *child_json = NULL;

    dp_info_json = ty_cJSON_Parse(dp_info_str);
    if(NULL == dp_info_json){
        PR_ERR("Json is null.");
        return OPRT_COM_ERROR;
    }

    child_json = ty_cJSON_GetObjectItem(dp_info_json, IS_LUA_KEY);
    if(NULL == child_json || child_json->type > ty_cJSON_True) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",IS_LUA_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->is_lua = child_json->type;
    PR_DEBUG("dp_info->is_lua:%d.", dp_info->is_lua);

    child_json = ty_cJSON_GetObjectItem(dp_info_json, FUN_KEY);
    if(NULL == child_json || child_json->type != ty_cJSON_String) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",FUN_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    snprintf(dp_info->fun, FUN_LEN_MAX, "%s", child_json->valuestring);
    PR_DEBUG("dp_info->fun:%s.", dp_info->fun);

    child_json = ty_cJSON_GetObjectItem(dp_info_json, PRO_INFO_KEY);
    if(NULL == child_json || child_json->type != ty_cJSON_Object) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",PRO_INFO_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->pro_info = ty_cJSON_PrintUnformatted(child_json);//记得释放
    PR_DEBUG("dp_info->pro_info:%s.", dp_info->pro_info);


    INFO_JSON_ERR:
        ty_cJSON_Delete(dp_info_json);

    return op_ret;
}


OPERATE_RET lua_com_dp_up_info_json_parase(IN CHAR_T *dp_info_str, OUT TY_LUA_DP_INFO_S *dp_info)
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *dp_info_json = NULL;
    ty_cJSON *child_json = NULL;

    dp_info_json = ty_cJSON_Parse(dp_info_str);
    if(NULL == dp_info_json){
        PR_ERR("Json is null.");
        return OPRT_COM_ERROR;
    }

    child_json = ty_cJSON_GetObjectItem(dp_info_json, IS_LUA_KEY);
    if(NULL == child_json || child_json->type > ty_cJSON_True) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",IS_LUA_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->is_lua = child_json->type;

    child_json = ty_cJSON_GetObjectItem(dp_info_json, FUN_KEY);
    if(NULL == child_json || child_json->type != ty_cJSON_String) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",FUN_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    snprintf(dp_info->fun, FUN_LEN_MAX, "%s", child_json->valuestring);

#if 0
    child_json = ty_cJSON_GetObjectItem(dp_info_json, PRO_INFO_KEY);
    if(NULL == child_json || child_json->type != ty_cJSON_Object) {
        PR_ERR("ty_cJSON_GetObjectItem error,str:%s",PRO_INFO_KEY);
        op_ret = OPRT_CJSON_GET_ERR;
        goto INFO_JSON_ERR;
    }
    dp_info->pro_info = ty_cJSON_PrintUnformatted(child_json);//记得释放

#endif
    INFO_JSON_ERR:
        ty_cJSON_Delete(dp_info_json);

    return op_ret;
}

lua_State *g_L = NULL;

OPERATE_RET lua_com_init(IN CHAR_T *lua_file, OUT lua_State **out_L)
{
    OPERATE_RET op_ret = OPRT_OK;
    lua_State *L = NULL;
    
    PR_DEBUG("lua_file:%s.", lua_file);
#if 0
    if(NULL == g_L){
        L = luaL_newstate();
        if(NULL == L){
            PR_ERR("luaL_newstate error.");
            return OPRT_MALLOC_FAILED;
        }
        
        PR_DEBUG("luaL_newstate end.");
        //lua_checkstack(g_L,60);//修改Lua栈大小
     
        luaL_openlibs(L);//运行Lua虚拟机
        PR_DEBUG("luaL_openlibs end.");
        g_L = L;
    }else{
        L = g_L;
        PR_DEBUG("luaL_openlibs exisit.");
    }
#else
    L = luaL_newstate();
    if(NULL == L){
        PR_ERR("luaL_newstate error.");
        return OPRT_MALLOC_FAILED;
    }
    
    PR_DEBUG("luaL_newstate end.");
    //lua_checkstack(L,60);//修改Lua栈大小
 
    luaL_openlibs(L);//运行Lua虚拟机
    PR_DEBUG("luaL_openlibs end.");
#endif
    if(op_ret=luaL_loadfile(L, lua_file)){//装载lua脚本
        PR_ERR("lua load file:%s error:%s.", lua_file, lua_tostring(L, -1));
        return OPRT_COM_ERROR;
    }else{
        PR_DEBUG("lua load file:%s OK.", lua_file);
        if(op_ret = lua_pcall(L,0,0,0)){//运行lua脚本
            PR_ERR("run lua file:%s error:%d, %s.n", lua_file, op_ret, lua_tostring(L, -1)); 
            return OPRT_COM_ERROR;
        }else{
            PR_DEBUG("run %s ok.", lua_file); 
        }
    }
    *out_L = L;

    return OPRT_OK;
}


OPERATE_RET lua_com_dp_down_parseData(lua_State *L, CHAR_T *dp_down_fun, TY_OBJ_DP_S *obj_dp, OUT CHAR_T **out_val, OUT INT_T *len)
{
    INT_T i = 0;
    INT_T table_size = 0;
    INT_T dpId = 0;
    INT_T type = 0;
    CHAR_T *out_data = NULL;
    
    dpId = obj_dp->dpid;
    type = obj_dp->type;
    
    PR_DEBUG("dp_down_fun %s.", dp_down_fun); 
    lua_getglobal(L, dp_down_fun); //获取lua函数 dp_down_fun 
    lua_newtable(L); //创建table
    
    lua_com_table_push_val(L, "dpId", dpId);
    lua_com_table_push_val(L, "type", type);

    switch(type){
        case PROP_BOOL:
            lua_com_table_push_bool(L, "val", obj_dp->value.dp_bool);
            break;
        case PROP_VALUE:
            lua_com_table_push_val(L, "val", obj_dp->value.dp_value);
            break;
        case PROP_ENUM:
            lua_com_table_push_val(L, "val", obj_dp->value.dp_enum);
            break;
        case PROP_BITMAP:
            lua_com_table_push_val(L, "val", obj_dp->value.dp_bitmap);
            break;
        case PROP_STR:
            lua_com_table_push_string(L, "val", obj_dp->value.dp_str);
            break;
        default:
            PR_ERR("dataType error:%d.\n", type);
            return OPRT_COM_ERROR;
    }


    if(lua_pcall(L, 1, 1, 0) != 0){//调用lua函数 dp_dowm
        PR_ERR("error running function %s: %s", dp_down_fun, lua_tostring(L, -1));//把错误码当字符串取出
        return OPRT_COM_ERROR;
    }

    if(LUA_TTABLE != lua_type(L, -1)){ //返回不是table(数组)报错
        PR_ERR("return type error,type:%d.\n", lua_type(L, -1));
        return OPRT_COM_ERROR;
    }
    
    //table_size = lua_rawlen(L, -1);//lua
    table_size = lua_objlen(L, -1);//elua
    PR_DEBUG("table size:  %d.", table_size);
    out_data = Malloc(table_size);
    if(NULL == out_data){
        PR_ERR("Malloc error.");
        return OPRT_MALLOC_FAILED;
    }
    
    for(i = 1; i <= table_size; i++){
        lua_rawgeti(L, -1, i);
        out_data[i-1] = (INT_T)lua_tointeger(L, -1);       

        lua_pop(L, 1);//把上次的值出栈
        
    }

    *out_val = out_data;
    *len = table_size;

    return OPRT_OK;
}

OPERATE_RET lua_com_get_dp_info(IN lua_State *L, CHAR_T *key_str, CHAR_T **out_val)
{
    CHAR_T *str = NULL;
    CHAR_T *out_str = NULL;
    
    lua_getglobal(L, key_str); 
    if(LUA_TSTRING != lua_type(L,-1)){
        PR_ERR("data type error:type:%d.", lua_type(L,-1));
        return OPRT_COM_ERROR;
    }
    
    str = lua_tostring(L,-1);
    if(NULL == str){
        PR_ERR("lua_tostring is null.");
        return OPRT_COM_ERROR;
    }
    
    out_str = Malloc(strlen(str)+1);
    if(NULL == out_str){
        PR_ERR("malloc error.");
        lua_pop(L, 1);
        return OPRT_MALLOC_FAILED;
    }
    
    memset(out_str, 0, strlen(str)+1);
    strncpy(out_str, str, strlen(str));
    PR_DEBUG("out_str = %s.", out_str);
    *out_val = out_str;
    
    lua_pop(L, 1);

    return OPRT_OK;
}

OPERATE_RET lua_dp_up_table_parseData(IN lua_State *L, IN CHAR_T *dp_up_fun, IN CHAR_T *key_str, IN CHAR_T *data,  IN INT_T len, OUT TY_OBJ_DP_S **dp_data, OUT INT_T *dp_num)
{
    INT_T i = 0;
    INT_T table_size = 0;
    TY_OBJ_DP_S *dp_data_tmp = NULL;
    
    lua_getglobal(L, dp_up_fun);       //获取lua函数 dp_up函数
    lua_pushstring(L, key_str);        //把key字符串入栈
    
    PR_DEBUG("return key_str:%s.", key_str);
    for(i=0; i<len; i++){
        lua_pushinteger(L, data[i]);   //将数组的数据入栈
    }
 
    if(lua_pcall(L, len+1, 1, 0) != 0){  //调用lua函数 dp_up， len+1入栈的参数，1返回的参数
        PR_ERR("error running function '%s': %s", dp_up_fun, lua_tostring(L, -1));//把错误码当字符串取出
        return OPRT_COM_ERROR;
    }

    if(LUA_TTABLE != lua_type(L, -1)){
        PR_ERR("return type error,type:%d.", lua_type(L, -1));
        lua_pop(L, 1);
        return OPRT_COM_ERROR;
    }

    //table_size = lua_rawlen(L, -1);//lua
    table_size = lua_objlen(L, -1);//elua
    PR_DEBUG("table size:  %d.", table_size);
    
    *dp_num = table_size;
    dp_data_tmp = (TY_OBJ_DP_S*)Malloc(SIZEOF(TY_OBJ_DP_S)*table_size);
    if(NULL == dp_data_tmp){
        PR_ERR("Malloc error.");
        lua_pop(L, 1);
        return OPRT_MALLOC_FAILED;
    }
    
    lua_pushnil(L);
    for(i = 1; i <= table_size; i++){
        lua_next(L, 1);//出栈key(占用上面的nil)，val(栈顶 table[i])
        lua_com_get_up_dp_info(L, &dp_data_tmp[i-1]);
    }
    *dp_data = dp_data_tmp;
    
    lua_pop(L, 1);//把最后一个key出栈（即原push nil位置）,此时是原来返回的大table
    lua_pop(L, 1);//把函数return 返回的table出栈，栈里是空的了

    return OPRT_OK;
}

OPERATE_RET lua_get_pro_data(IN lua_State *L, OUT CHAR_T **pro_data_json_str)
{
    return lua_com_get_pro_data(L, pro_data_json_str);
}
