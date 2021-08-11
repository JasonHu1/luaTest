#ifndef __DEBUG_H_
#define __DEBUG_H_

extern int g_debug_level;
enum{
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_MSGDUMP,
    LOG_EXCESSIVE,
};



#define vdbg_printf(_fmt_, ...)\
    do{\
        printf("[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__, ##__VA_ARGS__);\
    }while(0)

#define DEBUG(format,...) printf(""format"", ##__VA_ARGS__)


//general simple
#define vLOG_DEBUG(...)    do{if(g_debug_level>=LOG_DEBUG){ printf(__VA_ARGS__);}}while(0)
#define vLOG_INFO(...)     do{if(g_debug_level>=LOG_INFO){ printf(__VA_ARGS__);}}while(0)
#define vLOG_ERR(...)      do{if(g_debug_level>=LOG_ERROR){printf(__VA_ARGS__);}}while(0)
#define vLOG_WARN(...)     do{if(g_debug_level>=LOG_WARNING){printf(__VA_ARGS__);}}while(0)

//module detailedly
#define vLOG_APP(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("APP::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    
#define vLOG_MODULE1(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("MODULE1::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    
#define vLOG_MODULE2(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("MODULE2::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    
#define vLOG_MODULE3(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("MODULE3::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    


#endif//__DEBUG_H_
