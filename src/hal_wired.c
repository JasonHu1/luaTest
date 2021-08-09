/*
 * Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include "uni_log.h"
#include "tuya_os_adapt_wired.h"

#define ETH_DEV "enp0s3"

OPERATE_RET tuya_adapter_wired_get_ip(OUT NW_IP_S *ip)
{
    int sock = 0;
    char ipaddr[50] = {0};

    struct sockaddr_in *sin;
    struct ifreq ifr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         PR_ERR("socket create failed");
         return OPRT_COM_ERROR;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ETH_DEV, sizeof(ifr.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0 ) {
         PR_ERR("ioctl failed");
         close(sock);
         return OPRT_COM_ERROR;
    }

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    strcpy(ip->ip,inet_ntoa(sin->sin_addr)); 
    close(sock);

    return OPRT_OK;
}

BOOL_T tuya_adapter_wired_station_conn(VOID)
{
    int sock;
    struct sockaddr_in *sin;
    struct ifreq ifr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         PR_ERR("socket failed");
         return OPRT_COM_ERROR;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ETH_DEV, sizeof(ifr.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        PR_ERR("ioctl failed");
        close(sock);
        return FALSE;
    }

    close(sock);

    if ((ifr.ifr_flags & IFF_UP) == 0)
        return FALSE;

    return TRUE;
}

OPERATE_RET tuya_adapter_wired_get_mac(OUT NW_MAC_S *mac)
{
    int sock;
    struct sockaddr_in *sin;
    struct ifreq ifr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         PR_ERR("socket failed");
         return OPRT_COM_ERROR;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ETH_DEV, sizeof(ifr.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        PR_ERR("ioctl failed");
        close(sock);
        return FALSE;
    }

    memcpy(mac->mac, ifr.ifr_hwaddr.sa_data, sizeof(mac->mac));

    PR_DEBUG("WIFI MAC: %02X-%02X-%02X-%02X-%02X-%02X\r\n",
             mac->mac[0],mac->mac[1],mac->mac[2],mac->mac[3],mac->mac[4],mac->mac[5]);
    
    close(sock);

    return OPRT_OK;
}

/* 无需实现 */
OPERATE_RET tuya_adapter_wired_set_mac(IN CONST NW_MAC_S *mac)
{
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wired_if_connect_internet(BOOL_T *status)
{
    *status = TRUE;

    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wired_wifi_set_station_connect(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd)
{
	/* return hal_wifi_connect_station(ssid, passwd); */
	return OPRT_OK;
}

BOOL_T tuya_adapter_wired_wifi_need_cfg(VOID)
{
    return FALSE;
}

OPERATE_RET tuya_adapter_wired_wifi_station_get_conn_ap_rssi(OUT SCHAR_T *rssi)
{
    /* return hal_wifi_get_station_rssi(rssi); */
    *rssi = 99;
    
	return OPRT_OK;
}

OPERATE_RET tuya_adapter_wired_get_nw_stat(GW_BASE_NW_STAT_T *stat)
{
    *stat = 0;

    return OPRT_OK;
}