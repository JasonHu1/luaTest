#ifndef __TUYA_LUA_ZIGBEE_H
#define __TUYA_LUA_ZIGBEE_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_z3_zigbee_bash.h"
#include "tuya_lua.h"


#define EP_KYE        "ep"
#define CLUSTER_KEY   "clusterId"
#define IS_RSP_KEY    "is_rsp"
#define CMD_KEY       "cmd"
#define CMDTYPE_KEY   "cmdType"


//{\"ep\": 3,\"clusterId\": 6,\"cmd\":0,\"cmdType\":0}
typedef struct {
    UCHAR_T  ep;
    USHORT_T clusterId;
    UCHAR_T  cmd;
    UCHAR_T  cmdType;
}TY_LUA_ZE_DOWN_DP_INFO;


typedef struct {
    USHORT_T  c_id;//cluster id
    USHORT_T a_id;//attrbute id
    UINT_T  min;//min val
    UINT_T  max;//max val
    UCHAR_T type;//data type
    INT_T   val;//data
}TY_REP_TABLE;

typedef struct {
   TY_REP_TABLE *rep_table;
   INT_T rep_table_num;
   
}TY_LUA_ZE_PRO_DATA;

OPERATE_RET tuya_zigbee_lua_report(IN TuYaApsFrame *sTyApsFrameTmp);
VOID tuya_lua_zigbee_init(VOID);

#ifdef __cplusplus
}
#endif

#endif


