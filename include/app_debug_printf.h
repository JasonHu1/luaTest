#ifndef __DEBUG_H_
#define __DEBUG_H_


extern int g_debug_level;
enum{
    DBG_ERROR,
    DBG_WARNING,
    DBG_INFO,
    DBG_DEBUG,
    DBG_MSGDUMP,
    DBG_EXCESSIVE,
};



#define vdbg_printf(_fmt_, ...)\
    do{\
        printf("[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__, ##__VA_ARGS__);\
    }while(0)

#define DEBUG(format,...) printf(""format"", ##__VA_ARGS__)


//general simple
#define vDBG_DEBUG(...)    do{if(g_debug_level>=DBG_DEBUG){ printf(__VA_ARGS__);printf("\n\r");}}while(0)
#define vDBG_INFO(...)     do{if(g_debug_level>=DBG_INFO){ printf(__VA_ARGS__);printf("\n\r");}}while(0)
#define vDBG_ERR(...)      do{if(g_debug_level>=DBG_ERROR){printf(__VA_ARGS__);printf("\n\r");}}while(0)
#define vDBG_WARN(...)     do{if(g_debug_level>=DBG_WARNING){printf(__VA_ARGS__);printf("\n\r");}}while(0)

//module detailedly
#define vDBG_APP(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("APP::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    
#define vDBG_MODULE1(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("MODULE1::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    
#define vDBG_MODULE2(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("MODULE2::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    
#define vDBG_MODULE3(level,_fmt_,...)\
    do{if(g_debug_level>=level){printf("MODULE3::[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__,##__VA_ARGS__);}}while(0)
    


#endif//__DEBUG_H_
