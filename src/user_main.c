#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "uni_log.h"

#include "tuya_iot_com_api.h"
#include "tuya_iot_sdk_api.h"
#include "tuya_iot_sdk_defs.h"
#include <errno.h>
#include "unit-test.h"
#include "modbus.h"
#include <sys/epoll.h>
#include "app_debug_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "app_defs_types.h"
#include "subdev.h"
#include "app_debug_printf.h"
#include "random-test-server.h"

#include "user_iot_intf.h"
#include "app_debug_printf.h"
#include "subdev.h"
#include "random-test-server.h"
#include "time_scale.h"

#define COUNT_NUM   1
#define EPOLL_TIMEOUT_MS    50



int g_debug_level=DBG_DEBUG;
unsigned int g_debug_module= MODULE_DBG_ALL;

int gSerialNm=1;
int gTcpClientNm=0;
int gTcpPiClientNm=0;
int gIdx=0;
LocalSocketRecord_t* gpTcpClientList = NULL;
modbus_t *ctx[MAX_EVENTS];
int isfirst=0;


const uint16_t UT_BITS_ADDRESS = 0x130;
const uint16_t UT_BITS_NB = 0x25;
const uint8_t UT_BITS_TAB[] = { 0xCD, 0x6B, 0xB2, 0x0E, 0x1B };

const uint16_t UT_INPUT_BITS_ADDRESS = 0x1C4;
const uint16_t UT_INPUT_BITS_NB = 0x16;
const uint8_t UT_INPUT_BITS_TAB[] = { 0xAC, 0xDB, 0x35 };

const uint16_t UT_REGISTERS_ADDRESS = 0x160;
const uint16_t UT_REGISTERS_NB = 0x3;
const uint16_t UT_REGISTERS_NB_MAX = 0x20;
const uint16_t UT_REGISTERS_TAB[] = { 0x022B, 0x0001, 0x0064 };

/* Raise a manual exception when this address is used for the first byte */
const uint16_t UT_REGISTERS_ADDRESS_SPECIAL = 0x170;
/* The response of the server will contains an invalid TID or slave */
const uint16_t UT_REGISTERS_ADDRESS_INVALID_TID_OR_SLAVE = 0x171;
/* The server will wait for 1 second before replying to test timeout */
const uint16_t UT_REGISTERS_ADDRESS_SLEEP_500_MS = 0x172;
/* The server will wait for 5 ms before sending each byte */
const uint16_t UT_REGISTERS_ADDRESS_BYTE_SLEEP_5_MS = 0x173;

/* If the following value is used, a bad response is sent.
   It's better to test with a lower value than
   UT_REGISTERS_NB_POINTS to try to raise a segfault. */
const uint16_t UT_REGISTERS_NB_SPECIAL = 0x2;

const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x108;
const uint16_t UT_INPUT_REGISTERS_NB = 0x1;
const uint16_t UT_INPUT_REGISTERS_TAB[] = { 0x000A };

const float UT_REAL = 123456.00;

const uint32_t UT_IREAL_ABCD = 0x0020F147;
const uint32_t UT_IREAL_DCBA = 0x47F12000;
const uint32_t UT_IREAL_BADC = 0x200047F1;
const uint32_t UT_IREAL_CDAB = 0xF1470020;

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

extern void* app_main_loop(void*args);

int main(int argc, char **argv)
{
    OPERATE_RET op_ret = OPRT_OK;
    pthread_t t;
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

    //message_encode();
    
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
    vDBG_APP(DBG_INFO,"app module debug level is info");
    vDBG_INFO("g_debug_module & MODULE_DBG_APP00 = %d",g_debug_module & MODULE_DBG_APP00);
    
    if(g_debug_module & MODULE_DBG_APP00){
        vDBG_INFO("open app module debug");
    }
    if(!(g_debug_module & MODULE_DBG_APP00)){
        vDBG_INFO("close app module debug");
    }
    
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
    
    timescale_init();

    {
        int s_tcp = -1,s_tcp_pi;
        modbus_t *ctx_tcp,*ctx_tcp_pi;
        modbus_mapping_t *mb_mapping;
        int rc;
        int i,j;
        int use_backend;
        uint8_t *query;
        int header_length;

        socketSeverInit(1502);

        query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);

        /**********uart setting***********/
        UARTCFG_T *uartCfg=NULL;
        gSerialNm = user_get_uartConfigure(cfg_str,&uartCfg);
        if((0==gSerialNm) || (uartCfg == NULL)){
            vDBG_ERR("uart config information error");
            exit(-1);
        }
        for(i=0;i<gSerialNm;i++){
            vDBG_INFO("uartCfg[0].parity =%d,%c",uartCfg[i].parity,uartCfg[i].parity);
            if(BUS_PROTOCOL_MODBUS==uartCfg[i].busProto){//modbus
                ctx[FD_RANK_SERIAL_START+i] = modbus_new_rtu(uartCfg[i].devName,uartCfg[i].baud, uartCfg[i].parity, uartCfg[i].dataBit, uartCfg[i].stopBit);
                if(rc = modbus_connect(ctx[FD_RANK_SERIAL_START+i])==-1){
                    vDBG_ERR("ctx[%d] connect failed !!!fd=%d",FD_RANK_SERIAL_START+i,modbus_get_socket(ctx[FD_RANK_SERIAL_START+i]));
                    exit(-1);
                }
                modbus_set_debug(ctx[FD_RANK_SERIAL_START+i], TRUE);
                vDBG_INFO("connect fd=%d success",modbus_get_socket(ctx[FD_RANK_SERIAL_START+i]));
                if(pthread_create(&t,NULL,app_main_loop,NULL)!=0){
                    vDBG_ERR("pthread create failed");
                    exit(-1);
                }
            }else if(BUS_PROTOCOL_KNX==uartCfg[i].busProto){
            }else{
            }
        }
        if(0!=user_save_slaveList(cfg_str)){
            vDBG_ERR("can't load the slave device");
            exit(-1);
        }

        closure_test(NULL);
        exit(0);
        //start_slave_timeScale();
        
        mb_mapping = modbus_mapping_new_start_address(
            UT_BITS_ADDRESS, UT_BITS_NB,
            UT_INPUT_BITS_ADDRESS, UT_INPUT_BITS_NB,
            UT_REGISTERS_ADDRESS, UT_REGISTERS_NB_MAX,
            UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB);
        if (mb_mapping == NULL) {
            fprintf(stderr, "Failed to allocate the mapping: %s\n",
                    modbus_strerror(errno));
            for(i=0;i<COUNT_NUM;i++){
                modbus_free(ctx[i]);
            }
            return -1;
        }

        /* Examples from PI_MODBUS_300.pdf.
           Only the read-only input values are assigned. */

        /* Initialize input values that's can be only done server side. */
        modbus_set_bits_from_bytes(mb_mapping->tab_input_bits, 0, UT_INPUT_BITS_NB,
                                   UT_INPUT_BITS_TAB);

        /* Initialize values of INPUT REGISTERS */
        for (i=0; i < UT_INPUT_REGISTERS_NB; i++) {
            mb_mapping->tab_input_registers[i] = UT_INPUT_REGISTERS_TAB[i];
        }

        struct epoll_event readyEvents[MAX_EVENTS];
        int nfds, epollfd;
        // 创建一个epoll实例
        if ((epollfd = epoll_create(MAX_EVENTS)) == -1) {
            perror("epoll_create");
            exit(1);
        }
       
        
        for(i=0;i<gSerialNm;i++){
    //        add_epoll_fd(epollfd,modbus_get_socket(ctx[FD_RANK_SERIAL_START]));
        }
       

        for (;;) {
            gTcpClientNm = socketSeverGetNumClients();
            vDBG_INFO("gTcpClientNm=%d",gTcpClientNm);
            LocalSocketRecord_t *tsock=gpTcpClientList;
            for(i=0;i<gTcpClientNm;i++){
                add_epoll_fd(epollfd,tsock->socketFd);
                tsock= tsock->next;
            }
            if ((nfds = epoll_wait(epollfd, readyEvents, MAX_EVENTS, EPOLL_TIMEOUT_MS)) == -1) {
                perror("epoll_wait");
                close(epollfd);
                exit(1);
            }
            vDBG_MODULE1(DBG_MSGDUMP,"nfds=%d",nfds);
            if(nfds){
                for (int n = 0; n < nfds; n++) {
                    vDBG_INFO("readyEvents[%d].data.fd=%d",n,readyEvents[n].data.fd);
                    for(j=0;j<gSerialNm;j++){
                        if (readyEvents[n].data.fd == modbus_get_socket(ctx[FD_RANK_SERIAL_START+j])) {//ttyUSB0
                            rc = modbus_receive(ctx[FD_RANK_SERIAL_START+j], query);
                            if (rc > 0) {
                                /* rc is the query size */
                                modbus_reply(ctx[FD_RANK_SERIAL_START+j], query, rc, mb_mapping);
                            } else if (rc == -1) {
                                /* Connection closed by the client or error */
                               vDBG_ERR("modbus_receive failed !!!");
                            }
                        }
                        continue;
                    }
                    vDBG_INFO("222");
                    if (readyEvents[n].data.fd == gpTcpClientList->socketFd) {//new connect client socket fd
                        vDBG_INFO("000");
                        addTcpClientListRec(modbus_new_tcp(NULL, 1502));
                    }
                    vDBG_INFO("333");
                    //recevie data
                    if(NULL==gpTcpClientList){
                        continue;
                    }
                    LocalSocketRecord_t *iter=gpTcpClientList;
                    iter = iter->next;
                    do{
                        if(readyEvents[n].data.fd == iter->socketFd){
                            rc = modbus_receive(iter->context, query);
                            if (rc > 0) {
                                /* rc is the query size */
                                modbus_reply(iter->context, query, rc, mb_mapping);
                            } else if (rc == -1) {
                                /* Connection closed by the client or error */
                               // break;
                            }
                        }
                    }while(iter->next);
                    vDBG_INFO("444");
                }
            }else{//non-blocking poll
                sleep(1);
            }
            LocalSocketRecord_t *dsock=gpTcpClientList;
            for(i=0;i<gTcpClientNm;i++){
                rm_epoll_fd(epollfd,dsock->socketFd);
                dsock= dsock->next;
            }
        }
        printf("Quit the loop: %s\n", modbus_strerror(errno));
        modbus_mapping_free(mb_mapping);
        close(s_tcp);
        close(s_tcp_pi);
        for(i=0;i<COUNT_NUM;i++){
            modbus_close(ctx[i]);
            modbus_free(ctx[i]);
        }
        return 0;
    }

    return 0;
}
