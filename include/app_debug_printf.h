#ifndef __DEBUG_H_
#define __DEBUG_H_
#include "string.h"
#include "uni_log.h"
#ifdef __cplusplus
extern "C" {
#endif

extern int g_debug_level;
extern unsigned int g_debug_module;
#define MODULE_DBG_APP00            0x0001U
#define MODULE_DBG_MODULE01         0x0002U
#define MODULE_DBG_MODULE02         0x0004U
#define MODULE_DBG_MODULE03         0x0008U
#define MODULE_DBG_SDKCb04          0x0010U
#define MODULE_DBG_APP05            0x0020U
#define MODULE_DBG_APP06            0x0040U
#define MODULE_DBG_APP07            0x0080U
#define MODULE_DBG_APP08            0x0100U
#define MODULE_DBG_APP09            0x0200U
#define MODULE_DBG_APP10            0x0400U
#define MODULE_DBG_APP11            0x0800U
#define MODULE_DBG_APP12            0x1000U
#define MODULE_DBG_APP13            0x2000U
#define MODULE_DBG_APP14            0x4000U
#define MODULE_DBG_APP15            0x8000U
#define MODULE_DBG_ALL              0xFFFFU

enum{
    DBG_ERROR,
    DBG_WARNING,
    DBG_INFO,
    DBG_DEBUG,
    DBG_MSGDUMP,
    DBG_EXCESSIVE,
};

#define BASENAME(x)   strrchr(x,'/')?strrchr(x,'/')+1:x


#define BASENAME(x)   strrchr(x,'/')?strrchr(x,'/')+1:x


#define BASENAME(x)   strrchr(x,'/')?strrchr(x,'/')+1:x



#define vdbg_printf(_fmt_, ...)\
    do{\
        printf("[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__, ##__VA_ARGS__);\
    }while(0)

#define DEBUG(format,...) printf(""format"", ##__VA_ARGS__)
#define DEBUG_LOCATE(format,...)    printf("< JasonDbg >[%s:%d]%s() \r\n",__FILE__,__LINE__,__FUNCTION__);

//<>module name must three char

//general simple
#define vDBG_DEBUG(_fmt_,...)    do{if(g_debug_level>=DBG_DEBUG){  printf("< DBG >[%s:%d]%s() " _fmt_,BASENAME(__FILE__),__LINE__,__FUNCTION__,##__VA_ARGS__);printf("\n\r");}}while(0)
#define vDBG_INFO(_fmt_,...)     do{if(g_debug_level>=DBG_INFO){   printf("< INF >[%s:%d]%s() " _fmt_,BASENAME(__FILE__),__LINE__,__FUNCTION__,##__VA_ARGS__);printf("\n\r");}}while(0)
#define vDBG_ERR(_fmt_,...)      do{if(g_debug_level>=DBG_ERROR){  printf("< ERR >[%s:%d]%s() " _fmt_,BASENAME(__FILE__),__LINE__,__FUNCTION__,##__VA_ARGS__);printf("\n\r");}}while(0)
#define vDBG_WARN(_fmt_,...)     do{if(g_debug_level>=DBG_WARNING){printf("< WAR >[%s:%d]%s() " _fmt_,BASENAME(__FILE__),__LINE__,__FUNCTION__,##__VA_ARGS__);printf("\n\r");}}while(0)

//module detailedly
#define vDBG_APP(level,_fmt_,...)\
    do{if((g_debug_level>=level) && ( (g_debug_module) & MODULE_DBG_APP00 )){\
		printf("< APP >[%d]%s() "_fmt_"\n\r",__LINE__,__FUNCTION__,##__VA_ARGS__);}}while(0)
    
#define vDBG_MODULE1(level,_fmt_,...)\
    do{if((g_debug_level>=level) && ( (g_debug_module) & MODULE_DBG_MODULE01 )){\
		printf("< MODULE1 >[%d]%s() "_fmt_"\n\r",__LINE__,__FUNCTION__,##__VA_ARGS__);}}while(0)
    
#define vDBG_MODULE2(level,_fmt_,...)\
    do{if((g_debug_level>=level) && ( (g_debug_module) & MODULE_DBG_MODULE02 )){\
		printf("< MODULE2 >[%d]%s() "_fmt_"\n\r",__LINE__,__FUNCTION__,##__VA_ARGS__);}}while(0)
    
#define vDBG_MODULE3(level,_fmt_,...)\
    do{if((g_debug_level>=level)&& ( (g_debug_module) & MODULE_DBG_MODULE03 )){\
		printf("< MODULE3 >[%d]%s() "_fmt_"\n\r",__LINE__,__FUNCTION__,##__VA_ARGS__);}}while(0)
    
#define vDBG_SDKCb(level,_fmt_,...)\
        do{if((g_debug_level>=level) && ( (g_debug_module) & MODULE_DBG_SDKCb04 )){\
			printf("< SDK >[%d]%s() "_fmt_"\n\r",__LINE__,__FUNCTION__,##__VA_ARGS__);}}while(0)
        

#ifdef __cplusplus
        }
#endif

#endif//__DEBUG_H_
