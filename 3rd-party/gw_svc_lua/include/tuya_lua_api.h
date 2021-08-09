#ifndef __TUYA_LUA_ZIGBEE_ENGINE_H
#define __TUYA_LUA_ZIGBEE_ENGINE_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_lua_def.h"
#include "tuya_gw_subdev_api.h"
#include "gw_com_def.h"

TY_LUA_ADAPTER_S *tuya_lua_register_pro_api(TY_LUA_SUPPORT_PROTOCOL pro, TY_LUA_ADAPTER_S *pro_api);
OPERATE_RET tuya_lua_dp_down_api(IN CHAR_T *dev_id, TY_LUA_SUPPORT_PROTOCOL pro, IN TY_CMD_U *cmd, IN CHAR_T type);
OPERATE_RET tuya_lua_dp_up_api(IN CHAR_T *dev_id, IN CHAR_T *key_str, IN CHAR_T *data,  IN INT_T len, OUT TY_OBJ_DP_S **dp_data, OUT INT_T *dp_num);


#ifdef __cplusplus
}
#endif

#endif

