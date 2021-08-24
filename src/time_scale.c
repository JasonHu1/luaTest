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

void printLuaStack(lua_State *L){
    int nIndex;
    int nType;
    fprintf(stderr, "================栈顶================\n");
    fprintf(stderr, "   索引  类型          值\n");
    for( nIndex = lua_gettop(L); nIndex > 0; --nIndex){
        nType = lua_type(L, nIndex);
         fprintf(stderr,"   (%d)  %s         %s\n",nIndex, 
         lua_typename(L,nType), lua_tostring(L,nIndex));  
    }
    fprintf(stderr, "================栈底================\n");

}

static int readData(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
#if 0
    /*  表放在索引 't' 处 */
     lua_pushnil(L);  /* 第一个键 */
     while (lua_next(L, -2) != 0) {
       /* 使用 '键' （在索引 -2 处） 和 '值' （在索引 -1 处）*/
       printf("%s - %s\n",
              lua_typename(L, lua_type(L, -2)),
              lua_typename(L, lua_type(L, -1)));
       //拷贝一份 key 到栈顶，然后对它做 lua_tostring 就不会改变原始的 key 值了
       lua_pushvalue(L, -2);
       if(lua_typename(L, lua_type(L, -1))){
            printf("%s =  %d\r\n",lua_tostring(L,-1),lua_tointeger(L, -2));
       }else{
            printf("[%d] =  %d\r\n",lua_tonumber(L, -1),lua_tointeger(L, -2));
       }
       
       /* 移除 '值' ；保留 '键' 做下一次迭代 */
       lua_pop(L, 2);
     }
#else
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);


    vdbg_printf("fc=%d,addr=%d,cnt=%d",fc,addr,cnt)    ;
#endif
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
    int ret;
    TY_OBJ_DP_S *pSource,*pTarget;
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
    int error=luaL_dofile(L, "../../test.lua");
    if(error) {
        vDBG_ERR("Error: %s", lua_tostring(L,-1));
        return 1;
    }
#if 1
    //4.获得lua函数名并执行
    lua_getglobal(L,"report_dp");

    /*3.参数入栈*/
    lua_pushnumber(L, 10);          // 压入第一个参数  
    lua_pushnumber(L, 20);          // 压入第二个参数  

    if((ret = lua_pcall(L, 2, 0, 0))!=LUA_OK)//有2个入参数，0个返回值
    {
        const char *pErrorMsg = lua_tostring(L, -1);  
        vDBG_ERR("ret=%d,%s",ret,pErrorMsg);
        lua_close(L);  
        return ;  
    }
#endif
    /* 清除Lua */    
    lua_close(L); 
    
    return 0;
}
