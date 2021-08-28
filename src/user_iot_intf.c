#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "uni_log.h"
#include "ty_cJSON.h"
#include "base_os_adapter.h"
#include "tuya_cloud_base_defs.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_sdk_api.h"

#include "user_iot_intf.h"

STATIC ty_cJSON *ty_sdk_cfg = NULL;

STATIC VOID __gw_local_log_cb(OUT CHAR_T *path, IN CONST INT_T len)
{
    PR_DEBUG("gw local log cb");
}

STATIC OPERATE_RET __gen_gw_attr(IN ty_cJSON *cfg, INOUT GW_ATTACH_ATTR_T *attr, OUT UINT_T *num)
{
    UINT_T attr_num = 0, i = 0, array_num = 0;
    OPERATE_RET op_ret = OPRT_OK;

    ty_cJSON *js_user_cfg = ty_cJSON_GetObjectItem(cfg, "user");
    if (js_user_cfg != NULL) {
        array_num = ty_cJSON_GetArraySize(js_user_cfg);
        for(i = 0; i < array_num && attr_num < GW_ATTACH_ATTR_LMT; i++) {
            ty_cJSON *c_child = ty_cJSON_GetArrayItem(js_user_cfg, i);
            if(NULL == c_child) {
                PR_ERR("get array null");
                continue;
            }
            ty_cJSON *js_tp = ty_cJSON_GetObjectItem(c_child, "tp");
            ty_cJSON *js_ver = ty_cJSON_GetObjectItem(c_child, "sw_ver");
            if ((js_tp != NULL  && js_tp->type == ty_cJSON_Number) && \
                (js_ver != NULL && js_ver->type == ty_cJSON_String)) {
                attr[attr_num].tp = js_tp->valueint;
                strncpy(attr[attr_num].ver, js_ver ->valuestring, SW_VER_LEN);
                attr_num++;
            }
        }
    }
    *num = attr_num;

    return op_ret;
}

STATIC VOID get_nwk_stat(IN CONST SDK_NW_STAT_T stat)
{
    PR_DEBUG("online stat: %d", stat);
}

STATIC VOID get_wifi_nwk_stat(IN CONST SDK_WIFI_NW_STAT_T stat)
{
    PR_DEBUG("wifi stat: %d", stat);
}

OPERATE_RET user_svc_init(VOID *cb)
{
    OPERATE_RET ret = OPRT_OK;
    BOOL_T is_gw = TRUE;

    if (ty_sdk_cfg == NULL) {
        PR_ERR("param cfg is invalid");
        return OPRT_INVALID_PARM;       
    }

    tuya_iot_sdk_pre_init(is_gw);

    tuya_user_svc_init(cb);

    tuya_iot_gw_dev_hb_init();

    tuya_user_dev_svc_init(ty_sdk_cfg);

    return ret;
}

OPERATE_RET user_svc_start(VOID *cb)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T attr_num = 0, i = 0;

#if defined(WIFI_GW) && (WIFI_GW==1)
    GW_WF_START_MODE wifi_start_mode = WF_START_AP_ONLY;
    GW_WF_CFG_MTHD_SEL wifi_cfg      = GWCM_OLD;
#endif

    if (ty_sdk_cfg == NULL) {
        PR_ERR("param cfg is invalid");
        return OPRT_INVALID_PARM;       
    }

    GW_ATTACH_ATTR_T attr[GW_ATTACH_ATTR_LMT];
    memset(attr, 0, GW_ATTACH_ATTR_LMT * SIZEOF(GW_ATTACH_ATTR_T));

    // generate which sub-device protocol type is supported
    __gen_gw_attr(ty_sdk_cfg, attr, &attr_num);
    for (i = 0; i < attr_num; i++) {
        PR_DEBUG("tp: %d, ver: %s", attr[i].tp, attr[i].ver);
    }

    ty_cJSON *js_pid = ty_cJSON_GetObjectItem(ty_sdk_cfg, "pid");
    ty_cJSON *js_version = ty_cJSON_GetObjectItem(ty_sdk_cfg, "sw_ver");

#if defined(WIFI_GW) && (WIFI_GW==1)
    ty_cJSON *js_start_mode = ty_cJSON_GetObjectItem(ty_sdk_cfg, "start_mode");
    if (js_start_mode != NULL && js_start_mode->type == ty_cJSON_Number) {
        wifi_start_mode = js_start_mode->valueint;
    }

#if defined(GW_SUPPORT_WIRED_WIFI) && (GW_SUPPORT_WIRED_WIFI==1)
    tuya_iot_wired_wf_sdk_init(IOT_GW_NET_WIRED_WIFI, wifi_cfg, wifi_start_mode, \
                               js_pid->valuestring, js_version->valuestring,     \
                               attr, attr_num);
#else
    tuya_iot_wf_sdk_init(wifi_cfg, wifi_start_mode,                    \
                         js_pid->valuestring, js_version->valuestring, \
                         attr, attr_num);
#endif
#else
    tuya_iot_sdk_init(js_pid->valuestring, js_version->valuestring, \
                      attr, attr_num);
#endif

    tuya_iot_sdk_reg_net_stat_cb(get_nwk_stat, get_wifi_nwk_stat);

    tuya_user_svc_start(cb);

    /* 用户业务 */
    tuya_user_dev_svc_start(ty_sdk_cfg);

    return op_ret;
}

/**
 * json config:
 * {
 *     "pid": <xxx>,
 *     "uuid": <...>,
 *     "authkey_key": <...>,
 *     "ap_ssid": <... optional>,
 *     "ap_password": <... optional>,
 *     "log_level": <level>,
 *     "sw_ver": <xxx>
 *     "storage_path": <...>,
 *     "start_mode": <xxxx>,
 *     "tuya": {
 *         "zigbee": {
 *                       "storage_path": <xxx, string>,
 *                       "cache_path": <xxx, string>,
 *                       "dev_name": <xxx, string>,
 *                       "cts": <xxx, number>,
 *                       "thread_mode": <xxx, number>,
 *                       "sw_ver": <xxx, string>
 *         },
 *         "bt": {
 *                   "enable_hb": <xxx, number>,
 *                   "scan_timeout": <xxx, number>,
 *                   "mode": <xxx, number>,
 *                   "sw_ver": <xxx, string>
 *         }
 *     },
 *     "user": {
 *         [ { "tp": <DEV_ATTACH_MOD_X, number>, "sw_ver": <version, string> }, ... ]
 *     },
 * }
 */
OPERATE_RET user_iot_init(IN CONST CHAR_T *str_cfg)
{
    OPERATE_RET op_ret = OPRT_OK;
#if defined(WIFI_GW) && (WIFI_GW==1)
    WF_GW_PROD_INFO_S prod_info = {0};
#else
    GW_PROD_INFO_S prod_info = {0};
#endif

    TY_IOT_APP_CBS_S iot_app_cbs =  {
        .gw_app_log_path_cb = __gw_local_log_cb,
    };

    tuya_os_intf_init();

    ty_sdk_cfg = ty_cJSON_Parse(str_cfg);
    if (ty_sdk_cfg == NULL) {
        PR_ERR("param cfg is invalid");
        return OPRT_INVALID_PARM;       
    }

    ty_cJSON *js_path = ty_cJSON_GetObjectItem(ty_sdk_cfg, "storage_path");
    if (js_path == NULL  || js_path->type != ty_cJSON_String) {
        PR_ERR("param cfg is invalid");
        return OPRT_INVALID_PARM;
    }

    op_ret = tuya_iot_init(js_path->valuestring);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_iot_init err: %d", op_ret);
        return op_ret;
    }

    ty_cJSON *js_log_level = ty_cJSON_GetObjectItem(ty_sdk_cfg, "log_level");
    if (js_log_level && js_log_level->type == ty_cJSON_Number) {
        SET_PR_DEBUG_LEVEL(js_log_level->valueint);
    }

    ty_cJSON *js_uuid = ty_cJSON_GetObjectItem(ty_sdk_cfg, "uuid");
    ty_cJSON *js_authkey = ty_cJSON_GetObjectItem(ty_sdk_cfg, "authkey");
    prod_info.uuid = js_uuid->valuestring;
    prod_info.auth_key = js_authkey->valuestring;

#if defined(WIFI_GW) && (WIFI_GW==1)
    ty_cJSON *js_ssid = ty_cJSON_GetObjectItem(ty_sdk_cfg, "ap_ssid");
    ty_cJSON *js_password = ty_cJSON_GetObjectItem(ty_sdk_cfg, "ap_password");
    if ((js_ssid != NULL && js_ssid->type == ty_cJSON_String) && \
        (js_password != NULL  && js_password->type == ty_cJSON_String)) {
        prod_info.ap_ssid =  js_ssid->valuestring;
        prod_info.ap_passwd = js_password->valuestring;
    }
            
    op_ret = tuya_iot_set_wf_gw_prod_info(&prod_info);
#else
    op_ret = tuya_iot_set_gw_prod_info(&prod_info);
#endif
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_iot_set_gw_prod_info err: %d", op_ret);
        return op_ret;
    }

    tuya_iot_app_cbs_init(&iot_app_cbs);

    return op_ret;
}


