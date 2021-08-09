#if 0
#ifndef __TUYA_LUA_ZIGBEE_ENGINE_H
#define __TUYA_LUA_ZIGBEE_ENGINE_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_types.h"
#include "uni_log.h"


/*******************结构******************/
typedef struct {
    unsigned short profileId;
    unsigned short clusterId;
    unsigned char  ep;
    unsigned short groupId;
    unsigned char  cmdType;             // zcl frame type:0x01 is global , 0x02 is specific to a cluster
    unsigned char  command;             // zcl command id
    unsigned short payloadLen;          // the len of message(zcl PayLoad)
    unsigned char *payloadBuf;
}TY_LUA_ZIGBEE_DATA;


typedef struct {
    UCHAR_T  dpid;
    USHORT_T clusterId;
    UCHAR_T  ep;
    USHORT_T attrId;
    UCHAR_T  attrType;
    UCHAR_T  cmdType;
    UCHAR_T  cmd;
    BOOL_T  is_lua;
}TY_LUA_DP_DATA;



#ifdef __cplusplus
}
#endif

#endif
#endif
