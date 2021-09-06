//
// Created by zjs on 18-5-9.
//

#ifndef ZJS_MESSAGE_H
#define ZJS_MESSAGE_H

#include <string>
#include <list>
#include <map>
#include <vector>
#include "Json.h"


using namespace std;

namespace iot {
    namespace lua {

        using namespace iot::utils;
        /**
 * 用于转换消息
 */
        class Message {

        private:
            string type;
            string sno;
            string mac;
            string command;
            string attribute;

        public:
            Json *data;

            Message() {
                data = new Json();
            }

            ~Message() {
                if (data != NULL) {
                    delete data;
                }
            }

            string getType() const {
                return type;
            }

            void setType(string t) {
                type = t;
            }

            string getSno() const {
                return sno;
            }

            void setSno(string t) {
                sno = t;
            }


            string getMac() const {
                return mac;
            }

            void setMac(string t) {
                mac = t;
            }

            string getCommand() const {
                return command;
            }

            void setCommand(string t) {
                command = t;
            }

            string getAttribute() const {
                return attribute;
            }


            void setAttribute(string t) {
                attribute = t;
            }


            string toJSON() {
                string str;
                str.append("{");
                str.append(R"("type":")").append(type).append("\",");
                str.append("\"mac\":\"").append(mac).append("\",");
                str.append("\"sno\":\"").append(sno).append("\",");
                str.append("\"command\":\"").append(command).append("\",");
                str.append("\"attribute\":\"").append(attribute).append("\",");
                str.append("\"data\":").append(data->toJSON(true));
                str.append("}");
                return str;
            }

            /**
             * 按照固定的格式从json转换为消息对象
             * @param json
             */
            void fromJson(string json) {
                Json j;
                j.fromJSON(json.c_str());


            }

        };
    }
}


#endif //ZJS_MESSAGE_H
