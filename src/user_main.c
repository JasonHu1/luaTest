#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "uni_log.h"

#include "tuya_iot_com_api.h"
#include "tuya_iot_sdk_api.h"
#include "tuya_iot_sdk_defs.h"

#include "user_iot_intf.h"
#include "app_debug_printf.h"

STATIC CHAR_T *__parse_config_file(CONST CHAR_T *filename)
{
    FILE *file = NULL;
    ty_cJSON *cfg = NULL;
    UINT_T length = 0;
    CHAR_T *content = NULL;
    UINT_T read_chars = 0;

    if (filename == NULL) {
        return NULL;
    }

    file = fopen(filename, "rb");
    if (file == NULL) {
        goto out;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        goto out;
    }

    length = ftell(file);
    if (length < 0) {
        goto out;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        goto out;
    }

    content = (CHAR_T *)malloc((UINT_T)length + SIZEOF (""));
    if (content == NULL) {
        goto out;
    }

    read_chars = fread(content, 1, (UINT_T)length, file);
    if (read_chars != length) {
        free(content);
        content = NULL;
        goto out;
    }
    content[read_chars] = '\0';

out:
    if (file != NULL) {
        fclose(file);
    }

    return content;
}

STATIC VOID __gw_reset_cb(GW_RESET_TYPE_E type)
{
    PR_DEBUG("gw reset callback");
    // TODO
    if (GW_RESET_DATA_FACTORY != type) {
        exit(0);
    }

    return;
}

STATIC VOID __gw_reboot_cb(VOID)
{
    PR_DEBUG("gw reboot callback");
    // TODO
    exit(0);
}

STATIC VOID __gw_upgrade_cb(CONST FW_UG_S *fw)
{
    if (fw == NULL) {
        PR_ERR("invalid param");
        return;
    }

    PR_DEBUG("gw upgrade callback");
    PR_DEBUG("        tp: %d", fw->tp);
    PR_DEBUG("    fw_url: %s", fw->fw_url);
    PR_DEBUG("    sw_ver: %s", fw->sw_ver);
    PR_DEBUG("   fw_hmac: %s", fw->fw_hmac);
    PR_DEBUG(" file_size: %u", fw->file_size);

    return;
}

STATIC VOID __gw_active_stat_cb(GW_STATUS_E status)
{
    PR_DEBUG("gw active stat callback, registered: %d", status);

    return;
}
#if 0
STATIC VOID __gw_net_stat_cb(BOOL_T online)
{
    PR_DEBUG("gw net stat callback, online: %d", online);

    return;
}
#endif
VOID __dp_cmd_obj(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    PR_DEBUG("soc recv obj dp cmd, cmd_tp: %d, dtt_tp: %d, dp_cnt: %u", dp->cmd_tp, dp->dtt_tp, dp->dps_cnt);

    if (dp->cid != NULL) {
        PR_DEBUG("soc not have cid, %s", dp->cid);
        return;
    }

    UINT_T index = 0;
    for (index = 0; index < dp->dps_cnt; index++) {
        CONST TY_OBJ_DP_S *p_dp_obj = dp->dps + index;
        PR_DEBUG("idx: %d, dpid: %d, type: %d, ts: %u", index, p_dp_obj->dpid, p_dp_obj->type, p_dp_obj->time_stamp);
        switch (p_dp_obj->type) {
            case PROP_BOOL:     { PR_DEBUG("bool value: %d", p_dp_obj->value.dp_bool); break;}
            case PROP_VALUE:    { PR_DEBUG("int value: %d", p_dp_obj->value.dp_value); break;}
            case PROP_STR:      { PR_DEBUG("str value: %s", p_dp_obj->value.dp_str); break;}
            case PROP_ENUM:     { PR_DEBUG("enum value: %u", p_dp_obj->value.dp_enum); break;}
            case PROP_BITMAP:   { PR_DEBUG("bits value: 0x%X", p_dp_obj->value.dp_bitmap); break;}
            default:            { PR_DEBUG("idx: %d dpid: %d type: %d ts: %u is invalid", index, p_dp_obj->dpid, p_dp_obj->type, p_dp_obj->time_stamp); break;}
        }//end of switch
    }
    // TODO

    // simple test
    OPERATE_RET op_ret = dev_report_dp_json_async(dp->cid, dp->dps, dp->dps_cnt);
    if (OPRT_OK != op_ret) {
        PR_DEBUG("dev_report_dp_json_async err: %d", op_ret);
    }


}

VOID __dp_cmd_raw(IN CONST TY_RECV_RAW_DP_S *dp)
{
    PR_DEBUG("soc recv raw dp cmd, cmd_tp: %d, dtt_tp: %d, dpid:%d, len:%u", dp->cmd_tp, dp->dtt_tp, dp->dpid, dp->len);

    if (dp->cid != NULL) {
        PR_DEBUG("soc not have cid, %s", dp->cid);
        return;
    }

    // TODO
}

VOID __dp_cmd_query(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_DEBUG("soc recv dp query cmd");

    if (dp_qry->cid != NULL) {
        PR_DEBUG("soc not have cid, %s", dp_qry->cid);
        return;
    }

    if (dp_qry->cnt == 0) {
        // TODO
    } else {
        PR_DEBUG("dp query, cnt: %d", dp_qry->cnt);
        UINT_T index = 0;
        for (index = 0; index < dp_qry->cnt; index++) {
            PR_DEBUG("dp query, dpid: %d", dp_qry->dpid[index]);
            // TODO
        }
    }
}

extern int app_main_loop(void*args);

int main(int argc, char **argv)
{
    OPERATE_RET op_ret = OPRT_OK;
    CHAR_T *cfg_str = NULL;
    /*注册网关管理函数*/
    TY_GW_INFRA_CBS_S gw_cbs = {
        .gw_reset_cb       = __gw_reset_cb,
        .gw_upgrade_cb     = __gw_upgrade_cb,
        .gw_active_stat_cb = __gw_active_stat_cb,
        .gw_reboot_cb      = __gw_reboot_cb,
    };
    /*tuya dp数据接收回调函数*/
    TY_IOT_DP_CBS_S dp_cbs = {
        .obj   = __dp_cmd_obj,
        .raw   = __dp_cmd_raw,
        .query = __dp_cmd_query
    };

    char cfgfilePath[512]={0};
    vDBG_INFO("argc=%d",argc);
    for(int m=0;m<argc;m++){
        vDBG_INFO("argv[%d]=%s",m,argv[m]);
    }
    if(argc == 1){
        vDBG_ERR("Please give config.json path!!!");
        exit(-1);
    }else{
        sprintf(cfgfilePath,"%s/%s",argv[1],"config.json");
    }
    vDBG_APP(DBG_INFO,"cfgfilePath=%s",cfgfilePath);

    vDBG_INFO("%s",tuya_iot_get_sdk_info());
    cfg_str = __parse_config_file(cfgfilePath);
    if (cfg_str == NULL) {
        printf("parse json config failed\n");
        return 0;
    }

    op_ret = user_iot_init(cfg_str);
    if (op_ret != OPRT_OK) {
        PR_ERR("user_iot_init err: %d", op_ret);
        return op_ret;
    }

    op_ret = user_svc_init((VOID *)&gw_cbs);
    if (op_ret != OPRT_OK) {
        PR_ERR("user_svc_init err: %d", op_ret);
        return op_ret;
    }

    op_ret = user_svc_start(NULL);
    if (op_ret != OPRT_OK) {
        PR_ERR("user_svc_start err: %d", op_ret);
        return op_ret;
    }

    tuya_iot_reg_dp_cb(DP_GW, 0, &dp_cbs);
#if 1
    while (1) {
        sleep(10);
    }
#else
    app_main_loop(NULL);

#endif
    return 0;
}
