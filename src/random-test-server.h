#ifndef __RANDOM_TEST_SERVER_H__
#define __RANDOM_TEST_SERVER_H__
#include "app_debug_printf.h"

#define BUS_PROTOCOL_MODBUS         1
#define BUS_PROTOCOL_KNX            2


typedef struct _uartcfg{
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

extern LocalSocketRecord_t* gpTcpClientList;

int user_get_uartConfigure(IN CONST CHAR_T *str_cfg,UARTCFG_T**uartCfg);

#endif//__RANDOM_TEST_SERVER_H__

