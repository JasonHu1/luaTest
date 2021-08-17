#include "time_scale.h"
#include <pthread.h>
#include "app_debug_printf.h"



pthread_mutex_t mutex;
static TimeScaleListElement_t* timeScaleDoubleList=NULL;

static int timeScaleList_add(TimeScaleListElement_t*node)
{
    if(node==NULL){
        vDBG_ERR("add list node failed");
        return -1;
    }
    pthread_mutex_lock(&mutex);
    if(timeScaleDoubleList==NULL){
        timeScaleDoubleList = node;
        timeScaleDoubleList->next = NULL;
        timeScaleDoubleList->previous = NULL;
    }else{
        node->previous = NULL;
        node->next = timeScaleDoubleList;
        timeScaleDoubleList = node;
    }
    pthread_mutex_unlock(&mutex); 
    return 0;
}
static int timeScaleList_remove(TimeScaleListElement_t*node)
{
    if(node==NULL){
        vDBG_ERR("add list node failed");
        return -1;
    }
    pthread_mutex_lock(&mutex);
    if(timeScaleDoubleList==NULL){
        vDBG_WARN("timescale list is null");
        return -2;
    }else{
       node->previous->next = node->next;
       node->next->previous = node->previous;
       free(node);
    }
    pthread_mutex_unlock(&mutex); 
    return 0;
}


static int timescale_init(void)
{
    int res;
    res=pthread_mutex_init(&mutex, NULL);//初始化锁
    if (res != 0)
    {
        perror("rwlock initialization failed");
        exit(-1);
    }
    return 0;
}
int timescale_create(uint32 timeout,void *cb_param,TIMESCALE_MODE_T mode,timerfunctionCB *funcb)
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
    
    timeScaleList_add(p);
    return 0;
}

int timescale_task_loop(void*args){
    timerfunctionCB pFunc=NULL;
    timescale_init();
    for(;;){
        time_t seconds;
        seconds = time(NULL);
        vDBG_INFO("timestamp:%lld",seconds);
        pthread_mutex_lock(&mutex);
        TimeScaleListElement_t* pN = timeScaleDoubleList;
        sleep(1);
        if(pN==NULL){
            continue;
        }
        do{
            if(seconds >= pN->content->timeout){
                pFunc = pN->content->cb;
                (pFunc)(pN->content->cb_param);
                if(pN->content->mode == TIMER_SINGLE){
                  timeScaleList_remove(pN);
                }else{
                  pN->content->timeout = pN->content->Reloads + seconds;
                }
            }
            pN = pN->next;
        }while(pN!=NULL);
        pthread_mutex_unlock(&mutex);
    }
}

