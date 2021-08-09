

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
                        .beginClass <TuyaProtoElement> ("T")
                        .addConstructor <void (*) (void)> ()
                        .addFunction("func",&TuyaProtoElement::func)
                        .addProperty<unsigned int>("dpid", &TuyaProtoElement::get_dpid, &TuyaProtoElement::set_dpid)
                        .addProperty<unsigned char>("type", &TuyaProtoElement::get_type, &TuyaProtoElement::set_type)
                        .addProperty<int>("vint", &TuyaProtoElement::get_valueint, &TuyaProtoElement::set_valueint)
                        .addProperty<unsigned int>("venum", &TuyaProtoElement::get_valueenum, &TuyaProtoElement::set_valueenum)
                        .addProperty<string>("vstr", &TuyaProtoElement::get_valuestr, &TuyaProtoElement::set_valuestr)
                        .addProperty<int>("vbool", &TuyaProtoElement::get_valuebool, &TuyaProtoElement::set_valuebool)
                        .addProperty<unsigned int>("vbitmap", &TuyaProtoElement::get_valuebitmap, &TuyaProtoElement::set_valuebitmap)
                        .addProperty<unsigned int>("timestamp", &TuyaProtoElement::get_timeStamp, &TuyaProtoElement::set_timeStamp)
                        .endClass()

                        .beginClass<TuyaProto>("TuyaProto")
                        .addProperty<unsigned char>("cmd_tp", &TuyaProto::get_cmd_tp, &TuyaProto::set_cmd_tp)
                        .addProperty<unsigned char>("dtt_tp", &TuyaProto::get_dtt_tp, &TuyaProto::set_dtt_tp)
                        .addProperty<unsigned int>("cid", &TuyaProto::get_cid, &TuyaProto::set_cid)
                        .addProperty<unsigned int>("mb_id", &TuyaProto::get_mb_id, &TuyaProto::set_mb_id)
                        .addProperty<unsigned int>("dps_cnt", &TuyaProto::get_dps_cnt)
                        .addProperty ("cmd_tp", &TuyaProto::get_cmd_tp, &TuyaProto::set_cmd_tp)
                        
                        .addData("mpublic", &TuyaProto::m_public)
                        .addFunction("setData", &TuyaProto::setData)
                        .addFunction("getData", &TuyaProto::getData)
                        .endClass()  

                        .beginClass<ThirdFormat>("3rdFormat")
                        .addData("payload",&ThirdFormat::py)
                        .addData("retVal",&ThirdFormat::retVal)
                        .endClass()

                        .beginClass<Cdp>("Cdp")
                        .addData("dpid", &Cdp::dpid)
                        .endClass()
                  ;
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
