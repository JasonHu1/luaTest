#ifndef __TUYA_LUA__H
#define __TUYA_LUA__H

#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_lua_def.h"
#include "tuya_os_adapter.h"
#include "gw_com_def.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
// #include "lua_com_api.h"


#define DO_DOWN_KEY "dp_down_"
#define DO_UP_KEY "dp_up_"


typedef struct lua_adapter_list_s {
    struct lua_adapter_list_s *next;
    TY_LUA_ADAPTER_S lua_adapter_api;
    INT_T lua_pro;
}LUA_ADAPTER_LIST_S;


typedef struct {
    LUA_ADAPTER_LIST_S *lua_adapter_list;
    INT_T list_num;
    MUTEX_HANDLE mutex;
}LUA_ADAPTER_S;


LUA_ADAPTER_LIST_S *tuya_lua_get_register_pro(TY_LUA_SUPPORT_PROTOCOL pro);
OPERATE_RET tuya_lua_register_pro(TY_LUA_SUPPORT_PROTOCOL pro, TY_LUA_ADAPTER_S *pro_api);
OPERATE_RET tuya_lua_dp_down(IN CHAR_T *lua_file, IN TY_LUA_SUPPORT_PROTOCOL pro, IN TY_CMD_U *cmd, IN CHAR_T type);
OPERATE_RET tuya_lua_dp_up(IN CHAR_T *lua_file, IN CHAR_T *key_str, IN CHAR_T *data,  IN INT_T len, OUT TY_OBJ_DP_S **dp_data, OUT INT_T *dp_num);


#ifdef __cplusplus
}
#endif

#endif
