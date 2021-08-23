#include "time_scale.h"
#include <pthread.h>
#include "app_debug_printf.h"
#include "subdev.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"



pthread_mutex_t mutex_timescale;
static TimeScaleListElement_t* timeScaleDoubleList=NULL;

static int timeScaleList_add(TIMESCALE_PAYLOAD_T*node)
{
    if(node==NULL){
        vDBG_ERR("add list node failed");
        return -1;
    }
    TimeScaleListElement_t *p=malloc(sizeof(TimeScaleListElement_t));
    if(p==NULL){
        vDBG_ERR("malloc failed");
        free(node);
        return -1;
    }
    p->content = node;
    pthread_mutex_lock(&mutex_timescale);
    vDBG_INFO("p=%08x",p);
    if(timeScaleDoubleList==NULL){
        timeScaleDoubleList = p;
        timeScaleDoubleList->next = NULL;
        timeScaleDoubleList->previous = NULL;
    }else{
        p->previous = NULL;
        p->next = timeScaleDoubleList;
        timeScaleDoubleList->previous = p;
        timeScaleDoubleList = p;
    }
    pthread_mutex_unlock(&mutex_timescale); 
    return 0;
}


int timescale_init(void)
{
    int res;
    pthread_t t;
    res=pthread_mutex_init(&mutex_timescale, NULL);//初始化锁
    if (res != 0)
    {
        perror("rwlock initialization failed");
        exit(-1);
    }
    if(pthread_create(&t,NULL,timescale_task_loop,NULL)!=0){
        vDBG_ERR("timescale_task_loop pthread create failed");
        exit(-1);
    }
    return 0;
}
int timescale_create(uint32 timeout,void *cb_param,TIMESCALE_MODE_T mode,timerfunctionCB funcb)
{
    time_t seconds;
    seconds = time(NULL);

    TIMESCALE_PAYLOAD_T*p = (TIMESCALE_PAYLOAD_T*)malloc(sizeof(TIMESCALE_PAYLOAD_T));
    if(NULL == p){
        vDBG_ERR("timer create failed");
        return -1;
    }
    p->cb_param = cb_param;
    p->cb = funcb;
    p->mode = mode;
    p->timerId = seconds;
    p->timeout = timeout+seconds;
    p->Reloads = timeout;
    vDBG_INFO("seconds = %lld,%lld,&p=%08x,p->cb=%08x",p->timeout,seconds,p,p->cb);
    timeScaleList_add(p);
    return 0;
}

void * timescale_task_loop(void*args){
    time_t seconds;

    for(;;){
        seconds = time(NULL);
        vDBG_INFO("timestamp:%lld",seconds);
        TimeScaleListElement_t* pN = timeScaleDoubleList;
        TimeScaleListElement_t* pNode = NULL;
        sleep(1);
        if((pN==NULL)||(pN->content==NULL)){
            vDBG_WARN("continue");
            continue;
        }
        pthread_mutex_lock(&mutex_timescale);
        do{
            vDBG_INFO("pN->content->timeout:%lld,pN=%08x,pN->content=%08x",pN->content->timeout,pN,pN->content);
            if(seconds >= pN->content->timeout){
                if(pN->content->cb){
                    (pN->content->cb)(pN->content->cb_param);
                }else{
                    vDBG_ERR("cb function is null");
                }
                
                if(pN->content->mode == TIMER_SINGLE){
                   vDBG_ERR("1111");
                   if((pN->previous) && (pN->next)){
                       vDBG_ERR("5555");
                       pN->previous->next = pN->next;
                       pN->next->previous = pN->previous;
                   }else if((pN->previous ==NULL)&& (pN->next!=NULL)){
                        pN->next->previous =NULL;
                        timeScaleDoubleList = pN->next;
                        vDBG_ERR("6666");
                   }else if((pN->previous !=NULL)&& (pN->next==NULL)){
                        vDBG_ERR("8888");
                        pN->previous->next=NULL;
                   }else if((pN->previous ==NULL)&& (pN->next==NULL)){
                        timeScaleDoubleList=NULL;
                   }
                   
                   pNode = pN->next;
                   
                   free(pN->content);
                   free(pN);
                   pN = pNode;
                }else{
                  vDBG_ERR("2222");
                  pN->content->timeout = pN->content->Reloads + seconds;
                  pN = pN->next;
                }
            }else{
                pN = pN->next;
                vDBG_ERR("7777");
            }
            vDBG_ERR("4444");
        }while(pN!=NULL);
        vDBG_ERR("3333");
        pthread_mutex_unlock(&mutex_timescale);
        
    }
}
uint8_t buffer[];
static int readData(lua_State* L){
    int ret;
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tonumber(L, -1);
    lua_pop(L, 2);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tonumber(L, -1);
    lua_pop(L, 2);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tonumber(L, -1);
    lua_pop(L, 2);

    vdbg_printf("fc=%d,addr=%d,cnt=%d",fc,addr,cnt)    ;
    
    //01 (0x01) Read Coils
    modbus_set_slave(ctx[FD_RANK_SERIAL_START], SERVER_ID);
    if(ret = modbus_read_bits(ctx[FD_RANK_SERIAL_START],0, 10,buffer)!= -1){
        vDBG_INFO("modbus_read_bits ok1");
        for(int i=0;i<10;i++){
            printf("%02x ",buffer[i]);
        }
        printf("\r\n");
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }

}
TY_OBJ_DP_S source={
    .dpid=1,
    .type=2,
};
TY_OBJ_DP_S target;

int timer_60s_cb(void*param)
{
    vDBG_INFO("Enter CB function");
    //1.创建一个state
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        vDBG_ERR("New state fail");
        return -1;
    }
    luaL_openlibs(L);
    lua_register(L, "readData", readData);
    //2. 运行脚本
    luaL_dofile(L, "../../test.lua");
    /*3.userdata 参数入栈*/
    lua_pushlightuserdata(L, &source);
    lua_setglobal(L, "source"); 

    lua_pushlightuserdata(L, &target);
    lua_setglobal(L, "target"); 
    
    //4.获得lua函数名并执行
    lua_getglobal(L,"readData");
    lua_pcall(L, 0, 0, 0);
    return 0;
}
