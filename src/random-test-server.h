#ifndef __RANDOM_TEST_SERVER_H__
#define __RANDOM_TEST_SERVER_H__
#include "app_debug_printf.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hal_comm.h"
#include "modbus.h"




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

int user_get_slaveInfo(IN CONST CHAR_T *str_cfg);
int socketSeverInit( uint32 port );
unsigned int socketSeverGetNumClients(void);
int add_epoll_fd(int epollfd, int fd);
int rm_epoll_fd(int epollfd, int fd);
int addTcpClientListRec(modbus_t *ctx );

#endif//__RANDOM_TEST_SERVER_H__

