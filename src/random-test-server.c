/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
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

#define MAX_EVENTS 10
#define COUNT_NUM   1
#define EPOLL_TIMEOUT_MS    50

#define FD_RANK_SERIAL_START    0

int g_debug_level=DBG_DEBUG;
int gSerialNm=1;
int gTcpClientNm=0;
int gTcpPiClientNm=0;
int gIdx=0;
LocalSocketRecord_t*gpTcpClientList=NULL;


int socketSeverInit( uint32 port )
{
  struct sockaddr_in serv_addr;
  int stat, tr=1;

  if(gpTcpClientList == NULL)   
  {
  // New record
    LocalSocketRecord_t *lsSocket = malloc( sizeof( LocalSocketRecord_t ) );
  
    lsSocket->socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (lsSocket->socketFd < 0) 
    {
       DEBUG("ERROR opening socket\n");
       return -1;
    }
    // Set the socket option SO_REUSEADDR to reduce the chance of a 
    // "Address Already in Use" error on the bind
    setsockopt(lsSocket->socketFd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int));
    // Set the fd to none blocking
    fcntl(lsSocket->socketFd, F_SETFL, O_NONBLOCK);
      
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    stat = bind(lsSocket->socketFd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr));
    if ( stat < 0) 
    {
	  free(lsSocket);
      return -1;
    }
    vdbg_printf("create");
    //will have 5 pending open client requests
    listen(lsSocket->socketFd,5); 
    lsSocket->next = NULL;
    //Head is always the listening socket
    gpTcpClientList= lsSocket;  
	lsSocket = NULL;
  }
  return 0;
}

int addTcpClientListRec(modbus_t *ctx )
{
  int tr=1;
  LocalSocketRecord_t *srchRec;
  
  LocalSocketRecord_t *newSocket = malloc( sizeof( LocalSocketRecord_t ) );
  if(newSocket==NULL)
    {
    DEBUG("mem error!");
    }
  newSocket->context = ctx;
  vdbg_printf("newSocket->context=%08x",newSocket->context);
  //open a new client connection with the listening socket (at head of list)
  newSocket->clilen = sizeof(newSocket->cli_addr);
      
  //Head is always the listening socket
  newSocket->socketFd = accept(gpTcpClientList->socketFd, 
               (struct sockaddr *) (&(newSocket->cli_addr)), 
               (socklen_t *)&(newSocket->clilen));
  newSocket->bRemotectl=0;
  newSocket->IsVerified = 0;
   //DEBUG("connected\n");

  if (newSocket->socketFd < 0) 
  {
        DEBUG("ErrorNo %d,%s",errno,strerror(errno));
        DEBUG("ERROR on accept");
      free(newSocket);
      return -1;
  }

   // Set the socket option SO_REUSEADDR to reduce the chance of a 
   // "Address Already in Use" error on the bind
   setsockopt(newSocket->socketFd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int));
   // Set the fd to none blocking
   fcntl(newSocket->socketFd, F_SETFL, O_NONBLOCK);
   
   //DEBUG("New Client Connected fd:%d - IP:%s\n", newSocket->socketFd, inet_ntoa(newSocket->cli_addr.sin_addr));
   
   newSocket->next = NULL;
   
   //find the end of the list and add the record
   srchRec = gpTcpClientList;
   // Stop at the last record
   while ( srchRec->next )
     srchRec = srchRec->next;

   // Add to the list
   srchRec->next = newSocket; 
    DEBUG("new socket fd %d\n",newSocket->socketFd);
   return(newSocket->socketFd);
}


void deleteTcpClientListRec( int rmSocketFd )
{
  LocalSocketRecord_t *srchRec, *prevRec=NULL;

  // Head of the timer list
  srchRec = gpTcpClientList;        
  // Stop when rec found or at the end
  while ( (srchRec->socketFd != rmSocketFd) && (srchRec->next) )
  {
    prevRec = srchRec;  
    // over to next
    srchRec = srchRec->next;  
	
  }
  
  if (srchRec->socketFd != rmSocketFd)
  {
      return;    
  }
  
  // Does the record exist
  if ( srchRec )
  {               
    // delete the timer from the list
    if ( prevRec == NULL )      
    {
      //trying to remove first rec, which is always the listining socket
      return; 
    }
    
    //remove record from list    
    prevRec->next = srchRec->next;    
      
    close(srchRec->socketFd);
    free(srchRec);        
  }
}

unsigned int socketSeverGetNumClients(void)
{  
  unsigned int recordCnt=0;
  LocalSocketRecord_t *srchRec;
  
 // DEBUG("socketSeverGetNumClients++\n", recordCnt);
  
  // Head of the timer list
  srchRec = gpTcpClientList;
  
  if(srchRec==NULL)
  {
    return -1;
  }
    
  // Stop when rec found or at the end
  while ( srchRec )
  {  
 //   DEBUG("socketSeverGetNumClients: recordCnt=%d\n", recordCnt);
    srchRec = srchRec->next;  
//	DEBUG("socketSeverGetNumClients: \n");
    recordCnt++;      
  }
  
//  DEBUG("socketSeverGetNumClients %d\n", recordCnt);
  return (recordCnt);
}

int add_epoll_fd(int epollfd, int fd){
    struct epoll_event ev;
    ev.events = EPOLLIN;//default level trigger
    ev.data.fd = fd;
    printf("fd=%d\r",fd);
    sleep(2);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl");
        printf("fd1=%d\r",fd);
        close(epollfd);
        sleep(1);
        exit(1);
    }
    return 0;
}
int rm_epoll_fd(int epollfd, int fd){
    struct epoll_event ev;
    ev.events = EPOLLIN;//default level trigger
    ev.data.fd = fd;
    printf("fd=%d\r",fd);
    sleep(2);
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
        perror("epoll_ctl");
        printf("fd1=%d\r",fd);
        close(epollfd);
        sleep(1);
        exit(1);
    }
    return 0;
}

int app_main_loop(void*args)
{
    int s_tcp = -1,s_tcp_pi;
    modbus_t *ctx[MAX_EVENTS],*ctx_tcp,*ctx_tcp_pi;
    modbus_mapping_t *mb_mapping;
    int rc;
    int i,j;
    int use_backend;
    uint8_t *query;
    int header_length;

    socketSeverInit(1502);

    query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);
    ctx[FD_RANK_SERIAL_START] = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);
    modbus_set_slave(ctx[FD_RANK_SERIAL_START], SERVER_ID);

    vdbg_printf("11111");
    
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
    vDBG_APP(DBG_INFO,"11111");
    printf("%s<%d>\r",__FUNCTION__,__LINE__);
   
    
    for(i=0;i<gSerialNm;i++){
        rc = modbus_connect(ctx[FD_RANK_SERIAL_START+i]);
        modbus_set_debug(ctx[FD_RANK_SERIAL_START+i], TRUE);
        add_epoll_fd(epollfd,modbus_get_socket(ctx[FD_RANK_SERIAL_START]));
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
        vDBG_MODULE1(DBG_INFO,"nfds=%d",nfds);
        if(nfds){
            vdbg_printf("000");
            for (int n = 0; n < nfds; n++) {
                vdbg_printf("readyEvents[%d].data.fd=%d",n,readyEvents[n].data.fd);
                for(j=0;j<gSerialNm;j++){
                    if (readyEvents[n].data.fd == modbus_get_socket(ctx[FD_RANK_SERIAL_START+j])) {//ttyUSB0
                        rc = modbus_receive(ctx[FD_RANK_SERIAL_START+j], query);
                        if (rc > 0) {
                            /* rc is the query size */
                            modbus_reply(ctx[FD_RANK_SERIAL_START+j], query, rc, mb_mapping);
                        } else if (rc == -1) {
                            /* Connection closed by the client or error */
                           // break;
                        }
                    }
                    continue;
                }
                vdbg_printf("222");
                if (readyEvents[n].data.fd == gpTcpClientList->socketFd) {//new connect client socket fd
                    vdbg_printf("000");
                    addTcpClientListRec(modbus_new_tcp(NULL, 1502));
                }
                vdbg_printf("333");
                //recevie data
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
                vdbg_printf("444");
            }
        }else{//non-blocking poll
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
