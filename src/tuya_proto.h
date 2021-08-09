#ifndef __TUYA_PROTO_H_
#define __TUYA_PROTO_H_

#include <iostream>
#include <string>
#include <string.h>
#include <list>


using namespace std;

class TuyaProtoElement{
    private:
        unsigned int m_dpid;
        unsigned char m_type;
        int m_valueint;
        unsigned int m_valueenum;
        std::string m_valuestr;
        int m_valuebool;
        unsigned int m_valuebitmap;
        unsigned int m_timeStamp;
    public:
        unsigned int get_dpid() const{
            return m_dpid;
        }
        void set_dpid(unsigned int d){
            m_dpid = d;
        }
        unsigned char get_type() const{
            return m_type;
        }
        void set_type(unsigned char d){
            m_type = d;
        }
        int get_valueint() const{
            return m_valueint;
        }
        void set_valueint(int d){
            m_valueint = d;
        }
        unsigned int get_valueenum() const{
            return m_valueenum;
        }
        void set_valueenum(unsigned int d){
            m_valueenum = d;
        }
        std::string get_valuestr() const{
            return m_valuestr;
        }
        void set_valuestr(std::string d){
            m_valuestr.swap(d);
        }
        int get_valuebool() const{
            return m_valuebool;
        }
        void set_valuebool(int d){
            m_valuebool = d;
        }
        unsigned int get_valuebitmap() const{
            return m_valuebitmap;
        }
        void set_valuebitmap(unsigned int d){
            m_valuebitmap = d;
        }
        unsigned int get_timeStamp() const{
            return m_timeStamp;
        }
        void set_timeStamp(unsigned int d){
            m_timeStamp = d;
        }     
        int func(){
            std::cout<<__FUNCTION__<<std::endl;
            return 0;
        }
};

class TuyaProto
{
    private:
        /** see DP_CMD_TYPE_E */
        unsigned char m_cmd_tp;
        /** see DP_TRANS_TYPE_T */
        unsigned char m_dtt_tp;
        /** if(NULL == cid) then then the cid represents gwid */
        unsigned int m_cid;
        /** mb id */
        unsigned int m_mb_id;
        /** the dp data */
    public:
        TuyaProto(){

        }
        ~TuyaProto(){

        }
        std::list<TuyaProtoElement>m_dpsList;
        int m_public;
        unsigned char get_cmd_tp() const{
            return m_cmd_tp;
        }
        void set_cmd_tp(unsigned char d){
            m_cmd_tp = d;
        }
        unsigned char get_dtt_tp() const{
            return m_dtt_tp;
        }
        void set_dtt_tp(unsigned char d){
            m_dtt_tp = d;
        }
        unsigned int get_cid() const{
            return m_cid;
        }
        void set_cid(unsigned int d){
            m_cid = d;
        }
        unsigned int get_mb_id() const{
            return m_mb_id;
        }
        void set_mb_id(unsigned int d){
            m_mb_id = d;
        }
        unsigned int get_dps_cnt() const{
            return m_dpsList.size();
        }        
        void setData(TuyaProtoElement d){
            bool found = false;
            unsigned char type = d.get_type();
            auto it = m_dpsList.begin();
            while (it != m_dpsList.end()) {
                if (d.get_dpid() == it->get_dpid()) {
                    found = true;
                    break;
                }
                it++;
            }
            if(found){
                if(type==0){
                    it->set_valuebool(d.get_valuebool());
                }else if(type==1){
                    it->set_valueint(d.get_valueint());
                }else if(type==2){
                    it->set_valuestr(d.get_valuestr());
                }else if(type==3){
                    it->set_valueenum(d.get_valueenum());
                }else if(type==4){
                    it->set_valuebitmap(d.get_valuebitmap());
                }
            } else {
                m_dpsList.push_back(d);
            }
        }
        TuyaProtoElement getData(){
            TuyaProtoElement element;
            if(!m_dpsList.empty()){
                element = m_dpsList.front();
                return element;
            }
        }
};


#endif//__TUYA_PROTO_H_
