#include "time_scale.h"
#include <pthread.h>
#include "app_debug_printf.h"
#include "subdev.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua_modbus.h"
#include "random-test-server.h"
#include "string.h"




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
    vDBG_TIMER(DBG_MSGDUMP,"p=%08x",p);
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
    vDBG_TIMER(DBG_MSGDUMP,"seconds = %lld,%lld,&p=%08x,p->cb=%08x",p->timeout,seconds,p,p->cb);
    timeScaleList_add(p);
    return 0;
}

void * timescale_task_loop(void*args){
    time_t seconds;

    for(;;){
        seconds = time(NULL);
        vDBG_TIMER(DBG_MSGDUMP,"timestamp:%lld",seconds);
        TimeScaleListElement_t* pN = timeScaleDoubleList;
        TimeScaleListElement_t* pNode = NULL;
        sleep(1);
        if((pN==NULL)||(pN->content==NULL)){
            vDBG_WARN("continue");
            continue;
        }
        pthread_mutex_lock(&mutex_timescale);
        do{
            vDBG_TIMER(DBG_MSGDUMP,"pN->content->timeout:%lld,pN=%08x,pN->content=%08x",pN->content->timeout,pN,pN->content);
            if(seconds >= pN->content->timeout){
                if(pN->content->cb){
                    (pN->content->cb)(pN->content->cb_param);
                }else{
                    vDBG_ERR("cb function is null");
                }
                
                if(pN->content->mode == TIMER_SINGLE){
                   if((pN->previous) && (pN->next)){
                       pN->previous->next = pN->next;
                       pN->next->previous = pN->previous;
                   }else if((pN->previous ==NULL)&& (pN->next!=NULL)){
                        pN->next->previous =NULL;
                        timeScaleDoubleList = pN->next;
                   }else if((pN->previous !=NULL)&& (pN->next==NULL)){
                        pN->previous->next=NULL;
                   }else if((pN->previous ==NULL)&& (pN->next==NULL)){
                        timeScaleDoubleList=NULL;
                   }
                   
                   pNode = pN->next;
                   
                   free(pN->content);
                   free(pN);
                   pN = pNode;
                }else{
                  pN->content->timeout = pN->content->Reloads + seconds;
                  pN = pN->next;
                }
            }else{
                pN = pN->next;
            }
        }while(pN!=NULL);
        pthread_mutex_unlock(&mutex_timescale);
        
    }
}
const MODBUS_FUNCTION_MAP_T gFunctionMap = {
    modbus_read_bits,
    modbus_read_input_bits,
    modbus_read_registers,
    modbus_read_input_registers,
    modbus_write_bit,
    modbus_write_register,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    modbus_write_bits,
    modbus_write_registers,
    modbus_report_slave_id,
    NULL,
    NULL,
    NULL,
    NULL,
    modbus_mask_write_register,
    modbus_write_and_read_registers
};

uint8_t buffer1[]={99,98,97,96,95,94,93};

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

TY_OBJ_DP_S source={
    .dpid=1,
    .type=2,
};
TY_OBJ_DP_S target;

int timer_60s_cb(void*param)
{
    int ret;
    TY_OBJ_DP_S *pSource,*pTarget;
    SLAVEINFOLIST_T *n = (SLAVEINFOLIST_T*)param;
    if(NULL==n){
        vDBG_ERR("cb param NULL");
        return -1;
    }
    vDBG_INFO("Enter CB function,pid=%s,slaveAddr=%d",n->pid,n->slave);
    //1.创建一个state
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        vDBG_ERR("New state fail");
        return -1;
    }
    //2.加载lua库
    luaL_openlibs(L);
    //3.声明C 扩展函数
    lua_register(L, "send_report",send_report);
    lua_register(L, "modbus_read_bits",__modbus_read_bits);
    lua_register(L, "modbus_read_input_bits",__modbus_read_input_bits);
    lua_register(L, "modbus_read_registers",__modbus_read_registers);
    lua_register(L, "modbus_read_input_registers",__modbus_read_input_registers);
    lua_register(L, "modbus_write_bit",__modbus_write_bit);
    lua_register(L, "modbus_write_register",__modbus_write_register);
    lua_register(L, "modbus_write_bits",__modbus_write_bits);
    lua_register(L, "modbus_write_registers",__modbus_write_bits);
    lua_register(L, "modbus_report_slave_id",__modbus_report_slave_id);
    lua_register(L, "modbus_mask_write_register",__modbus_mask_write_register);
    lua_register(L, "modbus_write_and_read_registers",__modbus_write_and_read_registers);
    
    //4. 运行脚本
    char path[512]={0};
    sprintf(path,"../../lua/%s.lua",n->pid);
    int error=luaL_dofile(L, path);
    if(error) {
        vDBG_ERR("Error: %s", lua_tostring(L,-1));
        return 1;
    }

    //5.获得lua函数名并执行
    lua_getglobal(L,"report_dp");

    char deviceId[64]={0};
    sprintf(deviceId,"channel_%d_slave_%d",n->channel,n->slave);

    /*6.参数入栈*/
    lua_pushinteger(L, n->slave);   // 压入第一个参数  
    lua_pushstring(L, deviceId);          // 压入第二个参数
    lua_pushinteger(L, n->channel);
    lua_pushlightuserdata(L,&source);
    //7.执行指定lua函数
    if((ret = lua_pcall(L, 4, 0, 0))!=LUA_OK)//有2个入参数，0个返回值
    {
        const char *pErrorMsg = lua_tostring(L, -1);  
        vDBG_ERR("ret=%d,%s",ret,pErrorMsg);
        lua_close(L);  
        return ;  
    }

    /* 清除Lua */    
    lua_close(L); 
    
    return 0;
}
