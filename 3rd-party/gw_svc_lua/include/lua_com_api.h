#ifndef __LUA_COM_API_H
#define __LUA_COM_API_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_z3_zigbee_bash.h"
#include "tuya_lua.h"

OPERATE_RET lua_com_dp_down_info_json_parase(IN CHAR_T *dp_info_str, OUT TY_LUA_DP_INFO_S *dp_info);

OPERATE_RET lua_com_dp_up_info_json_parase(IN CHAR_T *dp_info_str, OUT TY_LUA_DP_INFO_S *dp_info);

OPERATE_RET lua_com_init(IN CHAR_T *lua_file, OUT lua_State **out_L);



OPERATE_RET lua_com_dp_down_parseData(lua_State *L, CHAR_T *dp_down_fun, TY_OBJ_DP_S *obj_dp, OUT CHAR_T **out_val, OUT INT_T *len);


OPERATE_RET lua_com_get_dp_info(IN lua_State *L, CHAR_T *key_str, CHAR_T **out_val);


OPERATE_RET lua_dp_up_table_parseData(IN lua_State *L, IN CHAR_T *dp_up_fun, CHAR_T *key_str, IN CHAR_T *data,  IN INT_T len, OUT TY_OBJ_DP_S **dp_data, OUT INT_T *dp_num);


#ifdef __cplusplus
}
#endif

#endif


