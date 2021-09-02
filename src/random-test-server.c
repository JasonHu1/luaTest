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
#include "subdev.h"
#include "app_debug_printf.h"
#include "random-test-server.h"




SLAVEINFOLIST_T *gSlaveDeviceInfoList=NULL;






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
  vdbg_printf("newSocket->context=%p",newSocket->context);
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

