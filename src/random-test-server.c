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


#define MAX_EVENTS 10

int add_epoll_fd(int epollfd, int fd){
    struct epoll_event ev;
    ev.events = EPOLLIN;//default level trigger
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl");
        close(epollfd);
        exit(1);
    }
    return 0;
}
int main(int argc, char*argv[])
{
    int s_tcp = -1,s_tcp_pi;
    modbus_t *ctx[MAX_EVENTS];
    modbus_t * ctxtemp=malloc(1024);
    modbus_mapping_t *mb_mapping;
    int rc;
    int i;
    int use_backend;
    uint8_t *query;
    int header_length;

    if (argc > 1) {
        
    } else {
        /* By default */
        use_backend = TCP;
    }

    ctx[0] = modbus_new_tcp("127.0.0.1", 1502);
    query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);
    ctx[1] = modbus_new_tcp_pi("::0", "1502");
    ctx[2] = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);
    ctx[3] = modbus_new_rtu("/dev/ttyUSB1", 115200, 'N', 8, 1);
    modbus_set_slave(ctx[3], SERVER_ID);
    for(i=0;i<4;i++){
        modbus_set_debug(ctx[i], TRUE);
    }
    
    mb_mapping = modbus_mapping_new_start_address(
        UT_BITS_ADDRESS, UT_BITS_NB,
        UT_INPUT_BITS_ADDRESS, UT_INPUT_BITS_NB,
        UT_REGISTERS_ADDRESS, UT_REGISTERS_NB_MAX,
        UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        for(i=0;i<4;i++){
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
        mb_mapping->tab_input_registers[i] = UT_INPUT_REGISTERS_TAB[i];;
    }


    s_tcp = modbus_tcp_listen(ctx[0], 1);
    s_tcp_pi = modbus_tcp_pi_listen(ctx[1], 1);
    rc = modbus_connect(ctx[2]);
    rc = modbus_connect(ctx[3]);

    struct epoll_event readyEvents[MAX_EVENTS];
    int nfds, epollfd;
    // 创建一个epoll实例
    if ((epollfd = epoll_create(MAX_EVENTS)) == -1) {
        perror("epoll_create");
        exit(1);
    }
    for(i=0;i<4;i++){
        add_epoll_fd(epollfd,modbus_get_socket(ctx[i]));
    }

    for (;;) {
        // 等待epollfd表示的epoll实例中的事件变化，返回准备好的事件集合readyEvents
        if ((nfds = epoll_wait(epollfd, readyEvents, MAX_EVENTS, -1)) == -1) {
            perror("epoll_wait");
            close(epollfd);
            exit(1);
        }
        printf("nfds=%d\r\n",nfds);
        for (int n = 0; n < nfds; n++) {
            // 有新连接到来了
            if (readyEvents[n].data.fd == modbus_get_socket(ctx[0])) {//tcp
            }else if (readyEvents[n].data.fd == modbus_get_socket(ctx[1])) {//tcp_pi
            }else if (readyEvents[n].data.fd == modbus_get_socket(ctx[2])) {//ttyUSB0
                rc = modbus_receive(ctx[2], query);
                if (rc > 0) {
                    /* rc is the query size */
                    modbus_reply(ctx[2], query, rc, mb_mapping);
                } else if (rc == -1) {
                    /* Connection closed by the client or error */
                    break;
                }
            }else if (readyEvents[n].data.fd == modbus_get_socket(ctx[3])) {//ttyUSB1
                rc = modbus_receive(ctx[3], query);
                if (rc > 0) {
                    /* rc is the query size */
                    modbus_reply(ctx[3], query, rc, mb_mapping);
                } else if (rc == -1) {
                    /* Connection closed by the client or error */
                    break;
                }
            } else {//new connect client socket fd
            }
        }
        printf("Quit the loop: %s\n", modbus_strerror(errno));
        modbus_mapping_free(mb_mapping);
        close(s_tcp);
        close(s_tcp_pi);
        for(i=0;i<4;i++){
            modbus_close(ctx[i]);
            modbus_free(ctx[i]);
        }
        return 0;
    }
}
