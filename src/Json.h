//
// Created by zjs on 18-5-12.
//

#ifndef ZGATEWAY_JSON_H
#define ZGATEWAY_JSON_H

#include "parson.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

namespace iot {
    namespace utils {


/**
 * Json Node implemented by parson
 */
        class Json {
        private:
            JSON_Value *root;
            JSON_Value *pCurrent;
        public:


            Json() {
                root = NULL;
                pCurrent = NULL;
                object();
            }

            ~Json() {
                clear();
            }

            void list() {
                clear();
                root = json_value_init_array();
                pCurrent = root;
            }

            void object() {
                clear();
                root = json_value_init_object();
                pCurrent = root;
            }

            void clear() {
                if (root != NULL) {
                    json_value_free(root);
                    root = NULL;
                }
            }


            /**
             * 检查节点类型是否满足要求
             * @param pValue
             * @param type
             * @return
             */
            bool check_current(JSON_Value *pValue, JSON_Value_Type type) {
                if (pValue == NULL) {
                    cerr << "current node is NULL";
                    return false;
                }
                int t = json_type(pValue);
                if (type == t) {
                    return true;
                } else {
                    cerr << "need node type [" << type << ":" << t << "]\n";
                    return false;
                }
            }

            Json *f(string key, string v) {
                return str(key, v);
            }

            Json *f(string key, int v) {
                return integer(key, v);
            }

            Json *f(string key, bool v) {
                return boolean(key, v);
            }

            Json *f(string key, double v) {
                return number(key, v);
            }

            Json *f(string key, float v) {
                return number(key, v);
            }

            Json *fo(string key) {
                return obj(key);
            }

            Json *fl(string key) {
                return array(key);
            }

            Json *fp(string key, const char *v) {
                return strp(key, v);
            }

            Json *fp(string key, int v) {
                return integerp(key, v);
            }

            Json *fp(string key, bool v) {
                return booleanp(key, v);
            }

            Json *fp(string key, double v) {
                return numberp(key, v);
            }

            Json *fp(string key, float v) {
                return numberp(key, v);
            }

            Json *fop(string key) {
                return objp(key);
            }

            Json *flp(string key) {
                return arrayp(key);
            }

            Json *l(string v) {
                return lstr(v);
            }

            Json *l(int v) {
                return lint(v);
            }

            Json *l(bool v) {
                return lbool(v);
            }

            Json *l(double v) {
                return lnumber(v);
            }

            Json *l(float v) {
                return lnumber(v);
            }

            Json *lo() {
                return lobj();
            }


            Json *lp(string v) {
                return lstrp(v);
            }

            Json *lp(int v) {
                return lintp(v);
            }

            Json *lp(bool v) {
                return lboolp(v);
            }

            Json *lp(double v) {
                return lnumberp(v);
            }

            Json *lp(float v) {
                return lnumberp(v);
            }

            Json *lop() {
                return lobj();
            }


            int lcount() {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pa = json_value_get_array(pCurrent);
                    return json_array_get_count(pa);
                }
                return 0;
            }

            int fcount() {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pa = json_value_get_object(pCurrent);
                    return json_object_get_count(pa);
                }
                return 0;
            }

            Json *moveToRoot() {
                pCurrent = root;
                return this;
            }

            Json *find(string path) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pa = json_value_get_object(pCurrent);
                    JSON_Value *pFound = json_object_get_value(pa, path.c_str());
                    if (pFound) {
                        pCurrent = pFound;
                    }
                }
                return this;
            }

            Json *index(int i) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pa = json_value_get_array(pCurrent);
                    JSON_Value *pFound = json_array_get_value(pa, i);
                    if (pFound) {
                        pCurrent = pFound;
                    }
                }
                return this;
            }


            /**
             * 在数组中添加一个字符串,返回当前节点
             * @param key
             * @param v
             * @return
             */
            Json *lstr(string v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_string(v.c_str());
                    json_array_append_value(pObject, new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
                * 在数组中添加一个字符串,返回父节点
                * @param key
                * @param v
                * @return
                */
            Json *lstrp(string v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_string(v.c_str());
                    json_array_append_value(pObject, new_node);
                    pCurrent = new_node;
                }
                return this;
            }


            /**
             * 在数组中添加一个字符串,返回当前节点
             * @param v
             * @return
             */
            Json *lint(int v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_array_append_value(pObject, new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
             * 在数组中添加一个字符串,返回父节点
             * @param v
             * @return
             */
            Json *lintp(int v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_array_append_value(pObject, new_node);
                }
                return this;
            }


            /**
             * 将当前节点从JSON数种移除，返回一个新的对象
             * @return
             */
            Json *detachCurrent() {
                if (pCurrent == NULL) {
                    return NULL;
                } else {

                }
            }

            /**
             * 在数组中添加一个bool串,返回当前节点
             * @param v
             * @return
             */
            Json *lbool(bool v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_boolean(v);
                    json_array_append_value(pObject, new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
             * 在数组中添加一个bool,返回父节点
             * @param v
             * @return
             */
            Json *lboolp(bool v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_boolean(v);
                    json_array_append_value(pObject, new_node);

                }
                return this;
            }

            /**
             * 在数组中添加一个bool串,返回当前节点
             * @param v
             * @return
             */
            Json *lnumber(double v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_array_append_value(pObject, new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
             * 在数组中添加一个bool,返回父节点
             * @param v
             * @return
             */
            Json *lnumberp(double v) {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_array_append_value(pObject, new_node);

                }
                return this;
            }

            /**
             * 在数组中添加一个bool串,返回当前节点
             * @param v
             * @return
             */
            Json *lobj() {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_object();
                    json_array_append_value(pObject, new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
             * 在数组中添加一个obj,返回父节点
             * @param v
             * @return
             */
            Json *lobjp() {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_object();
                    json_array_append_value(pObject, new_node);

                }
                return this;
            }

            /**
             * 在数组中添加一个数组串,返回当前节点
             * @param v
             * @return
             */
            Json *ll() {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_array();
                    json_array_append_value(pObject, new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
             * 在数组中添加一个bool,返回父节点
             * @param v
             * @return
             */
            Json *llp() {
                bool check = check_current(pCurrent, JSONArray);
                if (check) {
                    JSON_Array *pObject = json_value_get_array(pCurrent);
                    JSON_Value *new_node = json_value_init_array();
                    json_array_append_value(pObject, new_node);

                }
                return this;
            }

            /**
             * 在Obj节点添加一个 Key value对,返回新的节点
             * @param key
             * @param v
             * @return
             */
            Json *str(string key, string v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_string(v.c_str());
                    json_object_set_value(pObject, key.c_str(), new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
                * 在Obj节点添加一个 Key value对,返回当前节点
                * @param key
                * @param v
                * @return
                */
            Json *strp(string key, string v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_string(v.c_str());
                    json_object_set_value(pObject, key.c_str(), new_node);
                }
                return this;
            }

            /**
             * 在Obj节点添加一个 Key value对,返回新的节点
             * @param key
             * @param v
             * @return
             */
            Json *integer(string key, int v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_object_set_value(pObject, key.c_str(), new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
                * 在Obj节点添加一个 Key value对,返回当前节点
                * @param key
                * @param v
                * @return
                */
            Json *integerp(string key, int v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_object_set_value(pObject, key.c_str(), new_node);
                }
                return this;
            }

            /**
            * 在Obj节点添加一个 Key value对,返回新的节点
            * @param key
            * @param v
            * @return
            */
            Json *boolean(string key, bool v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_boolean(v);
                    json_object_set_value(pObject, key.c_str(), new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
                * 在Obj节点添加一个 Key value对,返回当前节点
                * @param key
                * @param v
                * @return
                */
            Json *booleanp(string key, bool v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_boolean(v);
                    json_object_set_value(pObject, key.c_str(), new_node);
                }
                return this;
            }

            /**
           * 在Obj节点添加一个 Key value对,返回新的节点
           * @param key
           * @param v
           * @return
           */
            Json *number(string key, double v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_object_set_value(pObject, key.c_str(), new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
                * 在Obj节点添加一个 Key value对,返回当前节点
                * @param key
                * @param v
                * @return
                */
            Json *numberp(string key, double v) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_number(v);
                    json_object_set_value(pObject, key.c_str(), new_node);
                }
                return this;
            }

            /**
          * 在Obj节点添加一个 Key value对,返回新的节点
          * @param key
          * @param v
          * @return
          */
            Json *obj(string key) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_object();
                    json_object_set_value(pObject, key.c_str(), new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
                * 在Obj节点添加一个 Key value对,返回当前节点
                * @param key
                * @param v
                * @return
                */
            Json *objp(string key) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_object();
                    json_object_set_value(pObject, key.c_str(), new_node);

                }
                return this;
            }


            /**
         * 在Obj节点添加一个 Key value对,返回新的节点
         * @param key
         * @param v
         * @return
         */
            Json *array(string key) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_array();
                    json_object_set_value(pObject, key.c_str(), new_node);
                    pCurrent = new_node;
                }
                return this;
            }

            /**
                * 在Obj节点添加一个 Key value对,返回当前节点
                * @param key
                * @param v
                * @return
                */
            Json *arrayp(string key) {
                bool check = check_current(pCurrent, JSONObject);
                if (check) {
                    JSON_Object *pObject = json_value_get_object(pCurrent);
                    JSON_Value *new_node = json_value_init_array();
                    json_object_set_value(pObject, key.c_str(), new_node);
                }
                return this;
            }

            Json *parent() {
                if (pCurrent != NULL) {
                    pCurrent = pCurrent->parent;
                }
                return this;
            }

            bool fromJSON(const char *json) {
                clear();
                root = json_parse_string(json);
                pCurrent = root;
            }

            string toJSON(bool format) {
                string r;
                if (root == NULL) {
                    r = "{}";
                } else {
                    char *text = NULL;
                    if (format) {
                        text = json_serialize_to_string_pretty(root);
                    } else {
                        text = json_serialize_to_string(root);
                    }
                    r = text;

                    if (text) {
                        json_free_serialized_string(text);
                    }
                }
                return r;
            }

            void setStr(string value) {

                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            printf("error> str to object\n");
                            break;
                        case JSONArray:
                            printf("error> str to array\n");
                            break;
                        case JSONNumber:
                            double d;
                            d = strtod(value.c_str(), NULL);
                            pCurrent->value.number = d;
                            break;
                        case JSONString:
                            json_replace_string(pCurrent, value.c_str());
                            break;
                        case JSONBoolean:
                            if (value.length() == 0 || value.compare("0") == 0 || value.compare("false") == 0) {
                                pCurrent->value.boolean = false;
                            } else {
                                pCurrent->value.boolean = true;
                            }
                            break;
                        case JSONNull:

                            break;
                        case JSONError:
                            break;
                        default:
                            break;
                    }
                }
            }

            void setInt(int v) {

                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            printf("error> int to object\n");
                            break;
                        case JSONArray:
                            printf("error> int to array\n");
                            break;
                        case JSONNumber:
                            pCurrent->value.number = v;
                            break;
                        case JSONString:
                            char temp[100];
                            sprintf(temp, "%d", v);
                            json_replace_string(pCurrent, temp);
                            break;
                        case JSONBoolean:
                            if (v == 0) {
                                pCurrent->value.boolean = false;
                            } else {
                                pCurrent->value.boolean = true;
                            }
                            break;
                        case JSONNull:

                            break;
                        case JSONError:
                            break;
                        default:
                            break;
                    }
                }

            }

            void setNumber(double v) {

                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            printf("error> double to object\n");
                            break;
                        case JSONArray:
                            printf("error> double to array\n");
                            break;
                        case JSONNumber:
                            pCurrent->value.number = v;
                            break;

                        case JSONString:
                            char temp[100];
                            sprintf(temp, "%f", v);
                            json_replace_string(pCurrent, temp);
                            break;
                        case JSONBoolean:
                            if (fabsf(v) < 0.000000001f) {
                                pCurrent->value.boolean = false;
                            } else {
                                pCurrent->value.boolean = true;
                            }
                            break;
                        case JSONNull:

                            break;
                        case JSONError:
                            break;
                        default:
                            break;
                    }
                }

            }

            void setBool(bool v) {

                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            printf("error> boool to object\n");
                            break;
                        case JSONArray:
                            printf("error> bool to array\n");
                            break;
                        case JSONNumber:
                            pCurrent->value.number = v;
                            break;

                        case JSONString:
                            json_replace_string(pCurrent, v ? "true" : "false");
                            break;
                        case JSONBoolean:
                            pCurrent->value.boolean = v;
                            break;
                        case JSONNull:

                            break;
                        case JSONError:
                            break;
                        default:
                            break;
                    }
                }

            }

            string vstr() {
                char pTemp[200];
                string r;
                double d;
                char *pData;
                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            r = "object";
                            break;
                        case JSONArray:
                            r = "array";
                            break;
                        case JSONNumber:
                            d = json_value_get_number(pCurrent);
                            sprintf(pTemp, "%f", d);
                            r.append(pTemp);
                            break;
                        case JSONString:
                            pData = const_cast<char *>(json_value_get_string(pCurrent));
                            r.append(pData);
                            break;
                        case JSONBoolean:
                            r.append((json_value_get_boolean(pCurrent) == 0) ? "false" : "true");
                            break;
                        case JSONNull:
                            r.append("null");
                            break;
                        case JSONError:
                            r.append("error");
                            break;
                        default:
                            r.append("unknown");
                    }
                }

                return r;
            }

            int vint() {

                int r;
                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            r = 0;
                            break;
                        case JSONArray:
                            r = 0;
                            break;
                        case JSONNumber: {
                            double d = json_value_get_number(pCurrent);
                            r = floor(d);
                        }
                            break;
                        case JSONString: {
                            const char *pData = json_value_get_string(pCurrent);
                            atoi(pData);
                        }

                            break;
                        case JSONBoolean:
                            r = (json_value_get_boolean(pCurrent) == 0) ? false : true;
                            break;
                        case JSONNull:
                            r = 0;
                            break;
                        case JSONError:
                            r = 0;
                            break;
                        default:
                            r = 0;
                            break;
                    }
                }

                return r;
            }

            double vnumber() {

                double r;
                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            r = 0;
                            break;
                        case JSONArray:
                            r = 0;
                            break;
                        case JSONNumber:
                            r = json_value_get_number(pCurrent);

                            break;
                        case JSONString: {
                            const char *pData = json_value_get_string(pCurrent);
                            atof(pData);
                        }
                            break;
                        case JSONBoolean: {
                            r = (json_value_get_boolean(pCurrent) == 0) ? 0 : 1;
                        }

                            break;
                        case JSONNull:
                            r = 0;
                            break;
                        case JSONError:
                            r = 0;
                            break;
                        default:
                            r = 0;
                            break;
                    }
                }
                return r;
            }

            bool vbool() {

                bool r;
                int d;
                const char *pData;

                if (pCurrent != NULL) {
                    int type = json_value_get_type(pCurrent);
                    switch (type) {
                        case JSONObject:
                            r = false;
                            break;
                        case JSONArray:
                            r = false;
                            break;
                        case JSONNumber: {
                            d = json_value_get_number(pCurrent);
                            r = (d == 0) ? false : true;
                        }
                            break;
                        case JSONString: {
                            pData = json_value_get_string(pCurrent);
                            string temp = pData;
                            if (temp.compare("false") == 0 || temp.compare("0") == 0) {
                                r = false;
                            } else {
                                r = true;
                            }

                        }
                            break;
                        case JSONBoolean: {
                            r = (json_value_get_boolean(pCurrent) == 0) ? false : true;
                        }

                            break;
                        case JSONNull:
                            r = false;
                            break;
                        case JSONError:
                            r = false;
                            break;
                        default:
                            r = false;
                            break;
                    }
                }
                return r;
            }
        };

    }
}

#endif //ZGATEWAY_JSON_H
