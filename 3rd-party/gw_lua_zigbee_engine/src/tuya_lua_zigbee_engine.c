#if 0
#include "tuya_lua_zigbee_engine.h"


#define DP_DOWN "dp_down"
#define DP_UP   "dp_up"


//获取项里的值,栈顶为dpId的值，只取整型值 
STATIC VOID _get_number_index_param(lua_State *L, CHAR_T *index_str, INT_T *out_val)
{
    lua_pushstring(L, index_str);
    lua_gettable(L, -2);
    *out_val = lua_tointeger(L, -1);
    PR_DEBUG("%s: 0x%x.", index_str, *out_val);
    lua_pop(L, 1);
}


//获取栈dp_down上行数据固定转换关系
VOID lua_get_dp_down(lua_State *L, TY_LUA_DP_DATA *lua_dp_down_data)
{
    INT_T n = 0;
    INT_T i = 0;
    INT_T out_val = 0;
    
    lua_getglobal(L, DP_DOWN);// 获取脚本中的名为tbl的表    
    if(lua_istable(L, -1)){// 判断栈顶获取的变量是不是表
        n = lua_rawlen(L, -1);
        PR_DEBUG("table size:  %d", n);

        for(i = 1; i <= n; i++){
            
            lua_pushnumber(L, i);//先取表里项
            lua_gettable(L, -2);

            _get_number_index_param(L, "dpId", &out_val);//取表里每项里的具体值
            _get_number_index_param(L, "ep", &out_val);
            _get_number_index_param(L, "clusterId", &out_val);
            _get_number_index_param(L, "is_def_rsp", &out_val);
            _get_number_index_param(L, "is_lua", &out_val);

            lua_pop(L, 1);//把前一个项的table出栈
        }
    }

    return;
}
lua_State *g_L = NULL;

VOID lua_get_dp_up(lua_State *L, TY_LUA_DP_DATA *lua_dp_up_data)
{
    INT_T n = 0;
    INT_T i = 0;
    INT_T out_val = 0;
    
    lua_getglobal(L, DP_UP);// 获取脚本中的名为tbl的表
    
    if (lua_istable(L, -1)){// 判断栈顶获取的变量是不是表
        n = lua_rawlen(L, -1);
        PR_DEBUG("table size:  %d.", n);

        for(i = 1; i <= n; i++){
            
            lua_pushnumber(L, i);//先取表里项
            lua_gettable(L, -2);

            _get_number_index_param(L, "dpId", &out_val);
            lua_dp_up_data->dpid = out_val;
            _get_number_index_param(L, "ep", &out_val);
            lua_dp_up_data->ep = out_val;
            _get_number_index_param(L, "clusterId", &out_val);
            
            lua_dp_up_data->clusterId = out_val;
            _get_number_index_param(L, "attrId", &out_val);
            
            lua_dp_up_data->attrId = out_val;
            _get_number_index_param(L, "attr_type", &out_val);
            
            lua_dp_up_data->attrType = out_val;
            _get_number_index_param(L, "cmd_type", &out_val);
            
            lua_dp_up_data->cmdType = out_val;
            _get_number_index_param(L, "cmd", &out_val);
            
            lua_dp_up_data->cmd = out_val;
            _get_number_index_param(L, "is_lua", &out_val);
            
            lua_dp_up_data->is_lua = out_val;
            lua_pop(L, 1);//把前一个项的table出栈
        }
    }

    return;
}


INT_T lua_dp_up_parseData(char *array,  int array_num, int dp_id)
{
    char *dp_fun_str[32+1] = {0};
    int data_type = 0;
    int i = 0;
    int n = 0;
    INT_T val = 0;
    snprintf(dp_fun_str, 32, "dp%d_up_parseData", dp_id);
    lua_State *L = g_L;
    lua_getglobal(L, dp_fun_str);    // 获取lua函数 dp_up函数
 
    for(i=0; i<array_num; i++){
        printf("array[%d]:%d.\n",i, array[i]);
        lua_pushinteger(L, array[i]);//将数组的数据入栈
    }
 
    if(lua_pcall(L, array_num, 2, 0) != 0){//调用lua函数 dp_up
        printf("error running function '%s': %s", dp_fun_str, lua_tostring(L, -1));//把错误码当字符串取出
        return;
    }

    if(LUA_TNUMBER != lua_type(L, -1)){
        printf("return type error,type:%d.\n", lua_type(L, -1));
        return;
    }else{
        //data_type = lua_tointeger(L, -1);
        //lua_close(L);
        //return data_type;
        printf("data_type:%d.lua_type:%d.\n", data_type, lua_type(L, -2));
        if(PROP_BOOL == data_type){
            printf("bool data:0x%s.\n", lua_tostring(L, -2));
            val =  atoi(lua_tostring(L, -2));
            
            lua_close(L);
            return val;
        }else if(PROP_VALUE == data_type){
            printf("value data:%d.\n", lua_tonumber(L, -2));
        }else if(PROP_STR == data_type){
            printf("string data:%s.\n", lua_tostring(L, -2));
        }else if(PROP_ENUM == data_type){
            printf("enum data:%d.\n", lua_tointeger(L, -2));
        }else if(PROP_BITMAP == data_type){
            printf("bitmap data:%d.\n", lua_tointeger(L, -2));
        }else if(/*PROP_RAW*/ 5== data_type){
            n = lua_rawlen(L, -2);
            
            printf("table size:  %d.\n", n);
            for(i = 1; i <= n; i++){
                lua_rawgeti(L, -2, i);
                
                printf("lua_rawgeti[%d]:0x%x.\n", i, lua_tointeger(L, -1));
                
                lua_pop(L, 1);
            }

            //printf("lua_rawgeti[%d]:0x%x.\n", 1, lua_tointeger(L, -2));
            //printf("lua_rawgeti[%d]:0x%x.\n", 2, lua_tointeger(L, -3));
            return;
        }
    }
    
}


void lua_dp_down_parseData(lua_State *L, int dp_id)
{
    char *dp_fun_str[32+1] = {0};
    int data_type = 0;
    int i = 0;
    int n = 0;
    snprintf(dp_fun_str, 32, "dp%d_down_parseData", dp_id);
    lua_getglobal(L, dp_fun_str);    // 获取lua函数 dp_down函数
 
    
    //lua_pushinteger(L, PROP_BOOL);        //入栈数据类型
    //lua_pushinteger(L, 65535);        //入栈数据类型

    //lua_pushinteger(L, PROP_VALUE);        //入栈数据类型
    //lua_pushinteger(L, 65535);        //入栈数据类型

    lua_pushinteger(L, PROP_STR);        //入栈数据类型
    lua_pushstring(L, "0011223344");        //入栈数据类型

    if(lua_pcall(L, 2, 1, 0) != 0){//调用lua函数 dp_dowm
        printf("error running function '%s': %s", dp_fun_str, lua_tostring(L, -1));//把错误码当字符串取出
        return;
    }

    if(LUA_TTABLE != lua_type(L, -1)){
        printf("return type error,type:%d.\n", lua_type(L, -1));
        return;
    }else{
        n = lua_rawlen(L, -1);
        
        printf("table size:  %d.\n", n);
        for(i = 1; i <= n; i++){
            lua_rawgeti(L, -1, i);
            
            printf("lua_rawgeti[%d]:0x%x.\n", i, lua_tointeger(L, -1));
            
            lua_pop(L, 1);
        }

        //printf("lua_rawgeti[%d]:0x%x.\n", 1, lua_tointeger(L, -2));
        //printf("lua_rawgeti[%d]:0x%x.\n", 2, lua_tointeger(L, -3));
        return;
    }    
}

INT_T lua_dev_init(CHAR_T *lua_file, TY_LUA_DP_DATA *lua_dp_up_data, IN UCHAR_T *buffer)
{
    INT_T ret = 0;
    lua_State *L = luaL_newstate();//创建Lua栈
    g_L = L;
    //lua_checkstack(g_L,60);//修改Lua栈大小为60，防止在C和Lua之间传递大数据时，崩溃
 
    luaL_openlibs(L);//运行Lua虚拟机
    ret=luaL_loadfile(L, lua_file);
    if(0 != ret){//装载lua脚本
        PR_ERR("lua load file:%s error:%s.\n", lua_file, lua_tostring(L, -1));
        return -1;
    }else{
        PR_ERR("lua load file:%s OK.", lua_file);
        ret = lua_pcall(L,0,0,0);
        if(0 != ret){//运行lua脚本
            PR_DEBUG("run lua file:%s error:%d, %s.\n", lua_file, ret, lua_tostring(L, -1)); 
            return -1;
        }else{
            PR_DEBUG("run %s ok.\n", lua_file); 
        }
    }

    
    //lua_get_dp_down(L);

    lua_get_dp_up(L, lua_dp_up_data);


    //lua_close(L);
    return 0;
}

INT_T test_lua_to_c(VOID)
{

    //lua_dev_init("/tuya/test.lua");
    return 0;
}
#endif
