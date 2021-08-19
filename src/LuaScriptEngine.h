

#ifndef __LUASCRIPTENGINE_H_
#define __LUASCRIPTENGINE_H_

#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <lua.hpp>
#include <mutex>


#include "LuaBridge.h"
#include "luademo.h"
#include "tuya_proto.h"
#include "third_format.h"
#include "Message.h"
#include "Json.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "tuya_cloud_com_defs.h"
#ifdef __cplusplus
}
#endif

#define _log(x...) std::cout
extern int sendPtr(TuyaProtoElement * pp);

namespace iot {
    namespace lua {

        class LuaScriptEngine {
        private:
            lua_State *L;

            /**
             * 执行线程锁
             */
            std::mutex mutexExecute;

        public:

            /*
             * 注册ScriptBuffer class for lua to use
             */
            void registerClass() {
                using namespace luabridge;

                getGlobalNamespace(L)
                .beginNamespace("test")
                    .beginClass<A>("A")
                        .addConstructor<void(*)(std::string)>()
                        .addFunction("getName", &A::getName)
                        .addFunction("printName", &A::printName)
                    .endClass()
                    .beginClass<TuyaProtoElement>("TuyaProtoElement")
                        .addConstructor<void(*)()>()
                        .addProperty ("dpid", &TuyaProtoElement::get_dpid, &TuyaProtoElement::set_dpid)
                        .addProperty ("type", &TuyaProtoElement::get_type, &TuyaProtoElement::set_type)
                        .addProperty ("valueint", &TuyaProtoElement::get_valueint, &TuyaProtoElement::set_valueint)
                        .addProperty ("valueenum", &TuyaProtoElement::get_valueenum, &TuyaProtoElement::set_valueenum)
                        .addProperty ("valuestr", &TuyaProtoElement::get_valuestr, &TuyaProtoElement::set_valuestr)
                        .addProperty ("valuebool", &TuyaProtoElement::get_valuebool, &TuyaProtoElement::set_valuebool)
                        .addProperty ("valuebitmap", &TuyaProtoElement::get_valuebitmap, &TuyaProtoElement::set_valuebitmap)
                        .addProperty ("timeStamp", &TuyaProtoElement::get_timeStamp, &TuyaProtoElement::set_timeStamp)
                        .addFunction("send",&TuyaProtoElement::send)
                    .endClass()
                    .addFunction("sendPtr",&sendPtr)
                    .beginClass<TuyaProto>("TuyaProto")
                        .addConstructor<void(*)()>()
                        .addData<int>("m_public", &TuyaProto::m_public)
                        .addProperty ("cmd_tp", &TuyaProto::get_cmd_tp, &TuyaProto::set_cmd_tp)
                        .addProperty ("dtt_tp", &TuyaProto::get_dtt_tp, &TuyaProto::set_dtt_tp)
                        .addProperty ("cid", &TuyaProto::get_cid, &TuyaProto::set_cid)
                        .addProperty ("mb_id", &TuyaProto::get_mb_id, &TuyaProto::set_mb_id)
                        .addProperty ("dps_cnt", &TuyaProto::get_dps_cnt)
                        .addFunction("setData", &TuyaProto::setData)
                        .addFunction("getData", &TuyaProto::getData)
                    .endClass()
                    .beginClass<ThirdFormat>("ThirdFormat")
                        .addConstructor<void(*)()>()
                        .addData<int>("retVal", &ThirdFormat::retVal)
                        .addData<std::string>("payload", &ThirdFormat::payload)
                        .addFunction("printpayload", &ThirdFormat::printpayload)
                        .addFunction("getpayload", &ThirdFormat::getpayload)
                        .addFunction("setpayload", &ThirdFormat::setpayload)
                        .addFunction("setname", &ThirdFormat::setname)
                    .endClass()
                .endNamespace();//.beginNamespace("test")
            }

            LuaScriptEngine() {
                L = luaL_newstate();
                luaL_openlibs(L);
                registerClass();
            }

            ~LuaScriptEngine() {
                if (L != nullptr) {
                    lua_close(L);
                }
            }

            bool compileScript(const char *script, const char *tag) {

                //LOCK L
                std::unique_lock<std::mutex> lock(mutexExecute);

                int error = luaL_loadbuffer(L, script, strlen(script), tag) || lua_pcall(L, 0, 0, 0);
                if (error) {
                    std::cout<<"ERROR:" << lua_tostring(L, -1);
                    return false;
                }
                return true;
            }
            template<typename s,typename t>
            int execute(std::string script, std::string method, s *pSource,t *pTarget) {

                if (method.length() == 0) {
                    _log(INFO) << "* execute lua script  with empty method" << std::endl;
                    return -1;
                }
                
                int error = luaL_loadbuffer(L, script.c_str(), script.length(), "tuya") || lua_pcall(L, 0, 0, 0);
                if (error) {
                    std::cout<<lua_tostring(L, -1);
                    return -1;
                }
                try {
                    luabridge::LuaRef encoder = luabridge::getGlobal(L, method.c_str());
                    encoder(pSource,pTarget);
                }
                catch (luabridge::LuaException const &e) {
                    std::cout<<e.what();
                    return -1;
                }
                return 0;
            }

        };
        class xx {
            char q;

        };
    }
}

#endif //__LUASCRIPTENGINE_H_
