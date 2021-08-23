#ifndef __RANDOM_TEST_SERVER_H__
#define __RANDOM_TEST_SERVER_H__
#include "app_debug_printf.h"

#define BUS_PROTOCOL_MODBUS         1
#define BUS_PROTOCOL_KNX            2


typedef struct _uartcfg{
    uint8 channel;
    uint8 busProto;
    uint8 devName[32];
    uint32 baud;
    uint8 parity;
    uint8 dataBit;
    uint8 stopBit;
}UARTCFG_T;

typedef struct
{
    void   *next;
    int socketFd;
    modbus_t * context;
    int bRemotectl;	
	socklen_t clilen;
	uint8_t IsVerified:1;
	struct sockaddr_in cli_addr;
} LocalSocketRecord_t;

typedef struct _slaveInfoList{
    struct _slaveInfoList *next;
    struct _slaveInfoList *previous;
    uint8 channel;//identy which port that bind
    uint8 pid[128];//use for get lua script
    uint8 devName[128];//device nick name for user change
    uint32 rptInterval;//report time interval
    uint8 address;
    void *pdata;
}SLAVEINFOLIST_T;


extern LocalSocketRecord_t* gpTcpClientList;

int user_get_uartConfigure(IN CONST CHAR_T *str_cfg,UARTCFG_T**uartCfg);
int user_get_slaveInfo(IN CONST CHAR_T *str_cfg);


#endif//__RANDOM_TEST_SERVER_H__

