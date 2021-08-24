
#include "LuaScriptEngine.h"
#include <stdio.h>
#include "luademo.h"
#include "Json.h"
#include "Message.h"
#include "tuya_proto.h"
#include "third_format.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "tuya_cloud_com_defs.h"
#ifdef __cplusplus
}
#endif

using namespace iot::lua;



#ifdef __cplusplus
extern "C" {
#endif

int message_encode(void){
    TuyaProto dp_data;
    TuyaProtoElement ele;
    ele.set_dpid(0x32);
    
    size_t rbytes=0,fsize=0;
    char *pbuf=NULL;
    dp_data.set_cmd_tp(0x30);
    dp_data.m_public = 0x31;
    dp_data.setData(ele);
    
    ThirdFormat tt;
    LuaScriptEngine engine;
    return 0;
    FILE *fp = fopen("../../test.lua", "r");
    std::cout<<"fp:"<<fp<<std::endl;
    printf("fp:%d\r\n",fp);
    if(fp){
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        fseek(fp, 0,SEEK_SET);
        pbuf = (char *)malloc(fsize);
        rbytes = fread (pbuf,1,fsize,fp);
        if(rbytes){
            printf("%s",pbuf);
            std::cout<<"------before lua parse--------"<<std::endl;
            std::cout<<"dp_data.m_public "<<dp_data.m_public<<std::endl;
            std::cout<<"dp_data.get_cmd_tp "<<dp_data.get_cmd_tp()<<std::endl;
            std::cout<<"dp_data.get_dps_cnt "<<dp_data.get_dps_cnt()<<std::endl;

            TuyaProtoElement el = dp_data.getData();
            std::cout<<"el.get_dpid "<<el.get_dpid()<<std::endl;

            engine.execute(pbuf, "report_dp", &dp_data,&tt);

            std::cout<<"\r\n------After lua parse--------"<<std::endl;
            std::cout<<"dp_data.m_public "<<dp_data.m_public<<std::endl;
            std::cout<<"dp_data.get_cmd_tp "<<dp_data.get_cmd_tp()<<std::endl;
            std::cout<<"dp_data.get_dps_cnt "<<dp_data.get_dps_cnt()<<std::endl;
            
            printf("tt.retVal=%d\r\n",tt.retVal);
            tt.printpayload();\
            printf("tt.name=%s\r\n",tt.name.c_str());
        }
    }else{
        vDBG_ERR("lua script file open fail");
        return -1;
    }
    
    return 0;
}
#ifdef __cplusplus
}
#endif