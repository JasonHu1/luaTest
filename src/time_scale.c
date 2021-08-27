#include "time_scale.h"
#include <pthread.h>
#include "app_debug_printf.h"
#include "subdev.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua_modbus.h"
#include "random-test-server.h"



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

static int readData(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
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

#if 0

    lua_newtable(L);
    for(int i=6;i>=0;i--){
        lua_pushinteger(L,i);//key
        lua_pushinteger(L,buffer1[i]);
        lua_settable(L,-3);
    }

    return 1;
#else
    //01 (0x01) Read Coils
    modbus_set_slave(ctx[FD_RANK_SERIAL_START], SERVER_ID);
    if(ret = modbus_read_bits(ctx[FD_RANK_SERIAL_START],0, 10,buffer1)!= -1){
        vDBG_INFO("modbus_read_bits ok1");
        for(int i=0;i<10;i++){
            printf("%02x ",buffer1[i]);
        }
        printf("\r\n");
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    
#endif
}
/*
param1:devid
param2:dp
param3:dp count
*/
int send_report(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    int dp_cnt=lua_tointeger(L, -1);
    lua_pushvalue(L,-3);
    const char *devid=lua_tostring(L,-1);
    lua_pop(L,1);
    vDBG_APP(DBG_DEBUG,"dp_cnt=%d,devid=%s",dp_cnt,devid);

    printf("stack -2 type=%s\r\n",lua_typename(L, lua_type(L, -2)));
    TY_OBJ_DP_S*dp_send=(TY_OBJ_DP_S*)malloc(dp_cnt*sizeof(TY_OBJ_DP_S));
    if(dp_send==NULL){
        lua_pushinteger(L,-1);
        return 1;
    }
    lua_pushvalue(L,-2);//把dp table放到stack top
    for(int i=0;i<dp_cnt;i++){
        lua_pushinteger(L,i+1);//lua array start index is 1
        lua_gettable(L,-2);//第一个嵌套table放到stack top

        lua_pushstring(L,"dpid");
        lua_gettable(L, -2);
        dp_send[i].dpid=lua_tointeger(L,-1);
        lua_pop(L, 1);
        vDBG_APP(DBG_DEBUG,"dp_send[%d].dpid=%d",i,dp_send[i].dpid);

        lua_pushstring(L,"type");
        lua_gettable(L, -2);
        dp_send[i].type=lua_tointeger(L,-1);
        lua_pop(L, 1);
        vDBG_APP(DBG_DEBUG,"dp_send[%d].type=%d",i,dp_send[i].type);

        lua_pushstring(L,"value");
        lua_gettable(L, -2);
        switch(dp_send[i].type){
            case PROP_BOOL:
                dp_send[i].value.dp_bool=lua_tointeger(L,-1);
                vDBG_APP(DBG_DEBUG,"dp_send[%d].value.dp_bool=%d",i,dp_send[i].value.dp_bool);
                break;
            case PROP_VALUE:
                dp_send[i].value.dp_value=lua_tointeger(L,-1);
                vDBG_APP(DBG_DEBUG,"dp_send[%d].value.dp_value=%d",i,dp_send[i].value.dp_value);
                break;
            case PROP_STR:
                dp_send[i].value.dp_str=lua_tostring(L,-1);
                vDBG_APP(DBG_DEBUG,"dp_send[%d].value.dp_str=%s",i,dp_send[i].value.dp_str);
                break;
            case PROP_ENUM:
                dp_send[i].value.dp_enum=lua_tointeger(L,-1);
                vDBG_APP(DBG_DEBUG,"dp_send[%d].value.dp_enum=%d",i,dp_send[i].value.dp_enum);
                break;
            case PROP_BITMAP:
                dp_send[i].value.dp_bitmap=lua_tointeger(L,-1);
                vDBG_APP(DBG_DEBUG,"dp_send[%d].value.dp_bitmap=%d",i,dp_send[i].value.dp_bitmap);
                break;
            default:
                break;
        }
        lua_pop(L, 2);//pop the vale and embed table
    }
    if((ret = dev_report_dp_json_async(devid,dp_send, dp_cnt))!=OPRT_OK){
        vDBG_ERR("ret=0x%04x",ret);
    }
    lua_pushinteger(L,ret);
    free(dp_send);
    return 1;
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
    lua_register(L, "readData", readData);
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
    sprintf(path,"../../%s.lua",n->pid);
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
    lua_pushlightuserdata(L,&source);
    //7.执行指定lua函数
    if((ret = lua_pcall(L, 3, 0, 0))!=LUA_OK)//有2个入参数，0个返回值
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

static int mytest(lua_State *L) {
  //获取上值
  int upv = (int)lua_tonumber(L, lua_upvalueindex(1));
  printf("get upvalue = %d\n", upv);
  //修改第一个upvalue的值
  upv += 5;
  lua_pushinteger(L, upv);
  lua_replace(L, lua_upvalueindex(1));
 
  //获取一般参数
  printf("get general param=%d\n", (int)lua_tonumber(L,1));
 
  return 0;
}

int closure_test(void*param)
{
    int ret;
    TY_OBJ_DP_S *pSource,*pTarget;
    //1.创建一个state
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        vDBG_ERR("New state fail");
        return -1;
    }
    luaL_openlibs(L);

    //设置Cclosure函数的上值
    lua_pushinteger(L,10);
    lua_pushinteger(L,11);
    lua_pushcclosure(L,mytest,2);
    lua_setglobal(L,"upvalue_test");

    //2. 运行脚本
    int error=luaL_dofile(L, "../../test_closure.lua");
    if(error) {
        vDBG_ERR("Error: %s", lua_tostring(L,-1));
        return 1;
    }

    //4.获得lua函数名并执行
    //获取fclosure上值的名称(临时值, 不带env)
    lua_getglobal(L, "l_counter");
    const char *name = lua_getupvalue(L, -1, 1);
    printf("%s\n", name);
 
    //设置fclosure上值
    lua_getglobal(L, "l_counter");
    lua_pushinteger(L,1000);
    name = lua_setupvalue(L, -2, 1);
    printf("%s\n", name);
 
    lua_getglobal(L,"ltest");
    lua_pcall(L,0,0,0);
    lua_getglobal(L,"ltest");
    lua_pcall(L,0,0,0);

    //获取fclosure的上值（带env）
    lua_getglobal(L, "g_counter");
    lua_getupvalue(L, -1, 1);

    //通过settable重新设置env中对应的值
    lua_pushstring(L, "gloval_upvalue");
    lua_pushinteger(L,10000);
    lua_settable(L,-3);

    lua_pushstring(L, "gloval_upvalue1");
    lua_pushinteger(L,20000);
    lua_settable(L,-3);

    lua_getglobal(L,"gtest");
    lua_pcall(L,0,0,0);
    lua_close(L);
    return 0;
}

