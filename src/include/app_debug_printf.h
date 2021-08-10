#ifndef __DEBUG_H_
#define __DEBUG_H_

extern int g_debugLevel;



#define vdbg_printf(_fmt_, ...)\
    do{\
        printf("[%s]<%d>"_fmt_"\n\r",__FUNCTION__,__LINE__, ##__VA_ARGS__);\
    }while(0)

#define DEBUG(format,...) printf(""format"", ##__VA_ARGS__)
#endif//__DEBUG_H_