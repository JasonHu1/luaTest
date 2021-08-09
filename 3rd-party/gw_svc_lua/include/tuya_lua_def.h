
#ifndef __TUYA_LUA_DEF_H
#define __TUYA_LUA_DEF_H

#ifdef __cplusplus
	extern "C" {
#endif

#include <string.h>
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "uni_log.h"

#define FUN_LEN_MAX 64
#define KEY_LEN_MAX 64

#define LUA_VER_LEN          8
#define LUA_MANU_NAME_LEN    32
#define LUA_MODE_ID_LEN      32
#define LUA_PRO_LEN          16
#define LUA_PID_LEN          16
#define LUA_PID_VER_LEN      16


#define IS_LUA_KEY    "is_lua"
#define FUN_KEY       "fun"
#define PRO_INFO_KEY  "pro_info"


typedef enum {
    TY_LUA_ZIGBEE,
    TY_LUA_BT,
} TY_LUA_SUPPORT_PROTOCOL;

typedef struct {
    CHAR_T key_str[KEY_LEN_MAX+1];
    CHAR_T *data;
    INT_T len;
}LUA_KEY_DATA_S;


typedef struct {
    BOOL_T is_lua;
    CHAR_T fun[FUN_LEN_MAX+1];
    CHAR_T *pro_info;
}TY_LUA_DP_INFO_S;

typedef OPERATE_RET (*LUA_DOWN)(CHAR_T *dev_id, CHAR_T *pro_data, CHAR_T *data, INT_T len);

typedef struct {
    LUA_DOWN dp_down;
}TY_LUA_ADAPTER_S;

typedef struct {
    CHAR_T lua_ver[LUA_VER_LEN+1];
    CHAR_T lua_manu_name[LUA_MANU_NAME_LEN+1];
    CHAR_T lua_mode_id[LUA_MODE_ID_LEN+1];
    CHAR_T lua_pro[LUA_PRO_LEN+1];
    CHAR_T lua_pid[LUA_PID_LEN+1];
    CHAR_T lua_pid_ver[LUA_PID_VER_LEN+1];
}LUA_HEARD_INFO_S;


#ifdef __cplusplus
}
#endif

#endif

