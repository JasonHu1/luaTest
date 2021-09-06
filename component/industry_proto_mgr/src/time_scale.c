#include "time_scale.h"
#include <pthread.h>
 
#include "subdev.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua_modbus.h"
#include "random-test-server.h"
#include "string.h"



char gScriptPath[256]={0};

pthread_mutex_t mutex_timescale;
static TimeScaleListElement_t* timeScaleDoubleList=NULL;

int set_script_path(char*path)
{
    strcpy(gScriptPath,path);
    return 0;
}
static int timeScaleList_add(TIMESCALE_PAYLOAD_T*node)
{
    if(node==NULL){
        PR_ERR("add list node failed");
        return -1;
    }
    TimeScaleListElement_t *p=malloc(sizeof(TimeScaleListElement_t));
    if(p==NULL){
        PR_ERR("malloc failed");
        free(node);
        return -1;
    }
    p->content = node;
    pthread_mutex_lock(&mutex_timescale);
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
        PR_ERR("timescale_task_loop pthread create failed");
        exit(-1);
    }
    return 0;
}
int timescale_create(uint32_t timeout,void *cb_param,TIMESCALE_MODE_T mode,timerfunctionCB funcb)
{
    time_t seconds;
    seconds = time(NULL);

    TIMESCALE_PAYLOAD_T*p = (TIMESCALE_PAYLOAD_T*)malloc(sizeof(TIMESCALE_PAYLOAD_T));
    if(NULL == p){
        PR_ERR("timer create failed");
        return -1;
    }
    p->cb_param = cb_param;
    p->cb = funcb;
    p->mode = mode;
    p->timerId = seconds;
    p->timeout = timeout+seconds;
    p->Reloads = timeout;
    timeScaleList_add(p);
    return 0;
}

void * timescale_task_loop(void*args){
    time_t seconds;

    for(;;){
        seconds = time(NULL);
        TimeScaleListElement_t* pN = timeScaleDoubleList;
        TimeScaleListElement_t* pNode = NULL;
        sleep(1);
        if((pN==NULL)||(pN->content==NULL)){
            PR_WARN("continue");
            continue;
        }
        pthread_mutex_lock(&mutex_timescale);
        do{
            if(seconds >= pN->content->timeout){
                if(pN->content->cb){
                    (pN->content->cb)(pN->content->cb_param);
                }else{
                    PR_ERR("cb function is null");
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
        PR_ERR("cb param NULL");
        return -1;
    }
     PR_DEBUG("Enter CB function,pid=%s,slaveAddr=%d",n->pid,n->slave);
    //1.创建一个state
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        PR_ERR("New state fail");
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
    sprintf(path,"%s/%s.lua",gScriptPath,n->pid);
    int error=luaL_dofile(L, path);
    if(error) {
        PR_ERR("Error: %s", lua_tostring(L,-1));
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
        PR_ERR("ret=%d,%s",ret,pErrorMsg);
        lua_close(L);  
        return 0;  
    }

    /* 清除Lua */    
    lua_close(L); 
    
    return 0;
}

void get_luaTable_to_jsonstr( lua_State* L, int idx,char*buf)
{
    unsigned int indx=0;
    if (!lua_istable(L, idx))
    {
        return;
    }
    else {
        PR_DEBUG("buf=%p",buf);
        indx=strlen(buf);
        PR_DEBUG("index=%d",indx);
        sprintf(buf+indx,"%s","{");
        indx=strlen(buf);
        printf("266 %d %ld\r\n",indx,strlen(buf));
        printf("%d // %s\r\n",indx,buf);
        // 将table拷贝到栈顶
        lua_pushvalue(L, idx);
        int it = lua_gettop(L);
        printf("it=%d\r\n",it);
        // 压入一个nil值，充当起始的key
        lua_pushnil(L);
        while (lua_next(L, it))
        {
            // 现在的栈：-1 => value; -2 => key; index => table
            // 拷贝一份 key 到栈顶，然后对它做 lua_tostring 就不会改变原始的 key 值了
            lua_pushvalue(L, -2);
            // 现在的栈：-1 => key; -2 => value; -3 => key; index => table
            if (!lua_istable(L, -2))
            {
                // 不是table就直接取出来就行
                const char* key = lua_tostring(L, -1);
                printf("284 %s\r\n",key);
                if(lua_isboolean(L, -2)){
                    sprintf(buf+indx,"\"%s\":%s,", key, lua_toboolean(L, -2) ? "1" : "0");
                    indx=strlen(buf);
                    printf("1 %d // %s\r\n",indx,buf);
                }else if(lua_isfunction(L, -2)){
                     sprintf(buf+indx,"\"%s\":%s,", key, "function");
                     indx=strlen(buf);
                     printf("2 %d // %s\r\n",indx,buf);
                }else{
                     int tm=lua_tointeger(L, -2);
                     printf("tm =%d\r\n",tm);
                     sprintf(buf+indx,"\"%s\":%d,", key,tm);
                     indx=strlen(buf);
                     printf("3 %d // %s \r\n",indx,buf);
                }
            }
            else
            {
                const char* key = lua_tostring(L, -1);
                sprintf(buf+indx,"\"%s\":",key);
                indx=strlen(buf);
                printf("307 %d // %s\r\n",indx,buf);
                // 此刻-2 => value
                
                PR_DEBUG("buf=%p",buf);
                get_luaTable_to_jsonstr(L, -2,buf);
                //lua_pop(L, 1);
                indx=strlen(buf);
                sprintf(buf+indx,",");
                indx=strlen(buf);
                printf("313 %d // %s\r\n",indx,buf);
            }
            
            // 弹出 value 和拷贝的 key，留下原始的 key 作为下一次 lua_next 的参数
            lua_pop(L, 2);
            // 现在的栈：-1 => key; index => table
        }
        // 现在的栈：index => table （最后 lua_next 返回 0 的时候它已经把上一次留下的 key 给弹出了）
//         printf(",");
         sprintf(buf+indx-1,"%s","}");
         indx=strlen(buf);
         printf("%d // %s\r\n",indx,buf);
        // 弹出上面被拷贝的table
        lua_pop(L, 1);
    }
}

int timer_70s_cb(void*param)
{
    int ret;
    TY_OBJ_DP_S *pSource,*pTarget;
    SLAVEINFOLIST_T *n = (SLAVEINFOLIST_T*)param;
    if(NULL==n){
        PR_ERR("cb param NULL");
        return -1;
    }
     PR_DEBUG("Enter CB function,pid=%s,slaveAddr=%d",n->pid,n->slave);
    //1.创建一个state
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        PR_ERR("New state fail");
        return -1;
    }
    //2.加载lua库
    luaL_openlibs(L);
    
    //4. 运行脚本
    char path[512]={0};
    sprintf(path,"%s/%s.lua",gScriptPath,n->pid);
    int error=luaL_dofile(L, path);
    if(error) {
        PR_ERR("Error: %s", lua_tostring(L,-1));
        return 1;
    }
    int m = lua_gettop(L);
    //5.获得lua函数名并执行
    lua_getglobal(L,"attr");
    if(!lua_istable(L,-1)){
        PR_ERR("stack top is not table,pls check variable is not global ?");
        return -1;
    }
    m = lua_gettop(L);
    PR_DEBUG("stack param count=%d",m);

//    printf_Lua_topTable(L);
    char buff[1024]={0};
    get_luaTable_to_jsonstr(L,1,buff);

    printf("%s\r\n",buff);
    ty_cJSON*obj_body = ty_cJSON_Parse(buff);
    if(obj_body ==NULL){
        PR_DEBUG("error");
        return -1;
    }
    ty_cJSON*objItem=NULL,*objFc=NULL,*objAddr=NULL,*objDpid=NULL,*objType=NULL;
    int i=1;
    do{
        char bufferi[2]={0};
        sprintf(bufferi,"%c",(i+0x30));
        objItem=ty_cJSON_GetObjectItem(obj_body,bufferi);
        i++;
        if(objItem){
            objFc=ty_cJSON_GetObjectItem(objItem, "fc");
            objAddr=ty_cJSON_GetObjectItem(objItem, "addr");
            objFc=ty_cJSON_GetObjectItem(objItem, "dpid");
            objAddr=ty_cJSON_GetObjectItem(objItem, "type");
            if(objFc){
                switch(objFc->valueint){
                    case 0x03:
                    app_modbus_read_registers(n->did,objAddr->valueint,objDpid->valueint,objType->valueint,n->slave,n->channel);
                    break;
                    default:
                    break;
                }
            }
        }
        
    }while(objItem!=NULL);


    /* 清除Lua */    
    lua_close(L); 
    
    return 0;
}

