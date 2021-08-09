#ifndef __USER_IOT_INTF_H__
#define __USER_IOT_INTF_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "ty_cJSON.h"

OPERATE_RET user_iot_init(IN CONST CHAR_T *cfg);

OPERATE_RET user_svc_init(VOID *cb);

OPERATE_RET user_svc_start(VOID *cb);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __USER_IOT_INTF_H__

