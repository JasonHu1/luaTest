/*
 * @Author: ruofei.wang@tuya.com
 * @Date: 2021-08-31 10:57:40
 * @LastEditTime: 2021-08-31 11:00:58
 * @LastEditors: Please set LastEditors
 * @Description: A common IPC API for Inter-process communication.
 */

#ifndef _IPC_STUB_H
#define _IPC_STUB_H

/* 枚举值用以代表IPC角色 */
typedef enum _IpcRole
{
    IPC_CLIENT = 1, /*IPC客户端*/
    IPC_SERVER = 2  /*IPC服务端*/
} IpcRole;

/* 传递的消息类型 */
typedef enum _MsgType
{
    GATEWAY_INFO = 1,  /*网关的基本信息*/
    GATEWAY_CONFIG = 2 /*网关的配置信息*/
} MsgType;

/* 传递消息的结构体 */
typedef struct buff
{
    MsgType msg_type;    /* 消息类型 */
    char msg_data[1024]; /* 消息内容 */
} ipc_buff;

/*
 *@brief  消息接收回调函数
 *@param  msg   接收的消息(结构体ipc_buff)
 *@return void
*/
typedef void (*MsgReceiverHanderCb)(void *msg);

/*
 *@brief  IPC通道初始化函数(应在注册接收回调函数之后调用)
 *@param  role  表明自身当前的IPC角色
 *@return void
*/
void ipc_init(IpcRole role);

/*
 *@brief  注册消息接收回调函数
 *@param  receiver_handler  回调函数
 *@param  dstRole   对端的IPC角色信息(若自身为IPC_CLIENT,则对端为IPC_SERVER,反之则互换.)
 *@return void
*/
void ipc_regist_receiver(MsgReceiverHanderCb receiver_handler, IpcRole dstRole);

/*
 *@brief  消息发送函数
 *@param  msg   发送的消息(结构体ipc_buff)
 *@return void
*/
void ipc_send(void *msg);

/*
 *@brief  IPC通道关闭函数
 *@return void
*/
void ipc_close();

#endif