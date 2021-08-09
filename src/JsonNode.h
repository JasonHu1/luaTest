//
// Created by zjs on 18-5-10.
//

#ifndef ZJS_JSONNODE_H
#define ZJS_JSONNODE_H

#include <string>
#include <vector>
#include <cmath>

using namespace std;

namespace iot {
    namespace lua {


#define NODE_STRING 0
#define NODE_NUMBER 1
#define NODE_INTEGER 2
#define NODE_BOOLEAN 3
#define NODE_OBJECT 4
#define NODE_ARRAY 5

        /**
         * 处理JSON
         * 节点数据
         *   toJSON()  [] {}  filed:[123,456]
         *   toValue() [] {} 123 "string" true false nil
         *
         *   obj.makeArray() []  //current node
         *   obj.makeObject  {}  //current node
         *
         *   obj.bool(key,valeu) //field
         *   obj.str(key,valeu)  //field
         *   obj.int(key,valeu)  //field
         *   obj.number(key,valeu)  //field
         *   obj.obj(key)         //obj field
         *   obj.array(key)         //array field
         *
         *   obj.put(int)           // item
         *   obj.put(str)           // item
         *   obj.put(bool)           // item
         *   obj.put(number)           // item
         *   obj.putObj()           // obj item
         *   obj.putArray()           // array item
         */
        class JsonNode {
        private :
            string key;
            double doubleValue;
            string strValue;
            unsigned char type;// 0 string 1 number 2 boolean 3 object 4 list
            vector<JsonNode *> objs;
            JsonNode *mParent;

            std::string escape(const std::string &input) {
                std::string output;
                output.reserve(input.length());

                for (std::string::size_type i = 0; i < input.length(); ++i) {
                    switch (input[i]) {
                        case '"':
                            output += "\\\"";
                            break;
                        case '/':
                            output += "\\/";
                            break;
                        case '\b':
                            output += "\\b";
                            break;
                        case '\f':
                            output += "\\f";
                            break;
                        case '\n':
                            output += "\\n";
                            break;
                        case '\r':
                            output += "\\r";
                            break;
                        case '\t':
                            output += "\\t";
                            break;
                        case '\\':
                            output += "\\\\";
                            break;
                        default:
                            output += input[i];
                            break;
                    }

                }

                return output;
            }


        public:
            JsonNode() {
                mParent = NULL;
                type = NODE_OBJECT;
            }

            ~JsonNode() {
                clear();
            }

            JsonNode *parent() {
                return mParent;
            }

            void clear() {
                for (int i = 0; i < objs.size(); i++) {
                    delete objs[i];
                    objs.clear();
                }
            }

            JsonNode *makeArray() {
                this->type = NODE_ARRAY;
                return this;
            }

            JsonNode *makeObj() {
                this->type = NODE_OBJECT;
                return this;
            }

            JsonNode *boolean(string key, bool v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_BOOLEAN;
                node->doubleValue = v ? 1 : 0;
                node->key = key;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *str(string key, string v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_STRING;
                node->strValue = v;
                node->key = key;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *integer(string key, int v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_INTEGER;
                node->doubleValue = v;
                node->key = key;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *number(string key, double v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_NUMBER;
                node->doubleValue = v;
                node->key = key;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *obj(string key) {
                JsonNode *node = new JsonNode();
                node->type = NODE_OBJECT;
                node->key = key;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *array(string key) {
                JsonNode *node = new JsonNode();
                node->type = NODE_ARRAY;
                node->key = key;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *putBool(bool v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_BOOLEAN;
                node->doubleValue = v ? 1 : 0;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *putStr(string v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_STRING;
                node->strValue = v;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *putInt(string key, int v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_INTEGER;
                node->doubleValue = v;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            JsonNode *putNumber(string key, double v) {
                JsonNode *node = new JsonNode();
                node->type = NODE_NUMBER;
                node->doubleValue = v;
                this->objs.push_back(node);
                node->mParent = this;
                return node;
            }

            JsonNode *putObj() {
                JsonNode *node = new JsonNode();
                node->type = NODE_OBJECT;
                this->objs.push_back(node);
                node->mParent = this;
                return node;
            }

            JsonNode *putArray() {
                JsonNode *node = new JsonNode();
                node->type = NODE_ARRAY;
                node->mParent = this;
                this->objs.push_back(node);
                return node;
            }

            string toJSON(bool format) {
                char temp[200];
                string r;
                if (key.length() > 0)
                    r.append("\"").append(key).append("\":");
                switch (type) {
                    case NODE_ARRAY:
                        r.append("[");
                        for (int i = 0; i < objs.size(); i++) {
                            JsonNode *p = objs[i];
                            if (i > 0) {
                                r.append(",");
                            }
                            r.append(p->toJSON(format));
                        }
                        r.append("]");
                        break;
                    case NODE_OBJECT:
                        r.append("{");

                        for (int i = 0; i < objs.size(); i++) {
                            JsonNode *p = objs[i];
                            if (i > 0) {
                                r.append(",");
                            }
                            if (p->key.length() == 0) {
                                r.append("\"");
                                r += i;
                                r.append("\":");
                            } else {
                                r.append("\"");
                                r.append(escape(p->key));
                                r.append("\":");
                            }
                            r.append(p->toJSON(format));
                        }
                        r.append("}");
                        break;
                    case NODE_NUMBER:
                        sprintf(temp, "%f", this->doubleValue);
                        r.append(temp);
                        break;
                    case NODE_STRING:
                        r.append("\"").append(escape(this->strValue)).append("\"");
                        break;
                    case NODE_INTEGER:;
                        sprintf(temp, "%.0f", this->doubleValue);
                        r.append(temp);
                        break;
                    case NODE_BOOLEAN:
                        if (this->doubleValue > 0.0) {
                            r.append("true");
                        } else {
                            r.append("false");
                        }
                        break;
                    default:
                        r = "\"\"";
                }
                return r;
            }

        };

    }
}

#endif //ZJS_JSONNODE_H
