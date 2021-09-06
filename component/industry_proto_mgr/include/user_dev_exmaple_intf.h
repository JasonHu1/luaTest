#ifndef __USER_DEV_EXMAPLE_INTF_H__
#define __USER_DEV_EXMAPLE_INTF_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "ty_cJSON.h"

OPERATE_RET tuya_user_dev_svc_init(IN ty_cJSON *cfg);

OPERATE_RET tuya_user_dev_svc_start(IN ty_cJSON *cfg);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __USER_DEV_EXMAPLE_INTF_H__