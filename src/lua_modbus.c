#include "lua_modbus.h"
#include "subdev.h"
#include "hal_comm.h"

//01 (0x01) Read Coils
int __modbus_read_bits(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);


    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vdbg_printf("fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);
    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }

    uint8 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_read_bits(conn->pConnCxt,addr, cnt,rdData)!= -1){
        vDBG_INFO("modbus_read_bits ok1");
        for(int i=0;i<cnt;i++){
            printf("%02x ",rdData[i]);
        }
        printf("\r\n");
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    
    //struct read data into lua table that return to lua
    lua_newtable(L);
    for(int i=cnt;i>=0;i--){
        lua_pushinteger(L,i);//key
        lua_pushinteger(L,rdData[i]);
        lua_settable(L,-3);
    }

    return 1;

}



int __modbus_read_input_bits(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vdbg_printf("fc=%d,addr=%d,cnt=%d",fc,addr,cnt)    ;
    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }

#if 1


    return 1;
#else
    //01 (0x01) Read Coils
    modbus_set_slave(ctx[FD_RANK_SERIAL_START], SERVER_ID);
    if(ret = modbus_read_bits(ctx[FD_RANK_SERIAL_START],0, 10,buffer)!= -1){
        vDBG_INFO("modbus_read_bits ok1");
        for(int i=0;i<10;i++){
            printf("%02x ",buffer[i]);
        }
        printf("\r\n");
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    
#endif
}


int __modbus_read_registers(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d,channel=%d",fc,addr,cnt,slave,channel);

    uint16 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    lua_newtable(L);

    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_read_registers(conn->pConnCxt,addr, cnt,rdData)!= -1){
        //struct read data into lua table that return to lua
        for(int i=0;i<cnt;i++){
            lua_pushinteger(L,i);//key
            lua_pushinteger(L,rdData[i]);
            lua_settable(L,-3);
        }
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    

    return 1;

}
int __modbus_read_input_registers(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);

    uint16 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }

    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }
    lua_newtable(L);
    
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_read_input_registers(conn->pConnCxt,addr, cnt,rdData)!= -1){
        //struct read data into lua table that return to lua
        for(int i=0;i<cnt;i++){
            lua_pushinteger(L,i);//key
            lua_pushinteger(L,rdData[i]);
            lua_settable(L,-3);
        }
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    

    return 1;

}
/* 05 (0x05) Write Single Coil */
int __modbus_write_bit(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    if(!lua_istable(L,-2)){
        vDBG_ERR("stack -2 is not table");
        return -1;
    }
    lua_pushinteger(L,1);
    lua_gettable(L,-2);
    int status =lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);

    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }

    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_write_bit(conn->pConnCxt,addr, status)!= -1){
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    
    return 1;

}
/* 06 (0x06) Write Single Register */
int __modbus_write_register(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-2)){
        vDBG_ERR("stack second is not table");
        return 0;
    }
    int registerVal=lua_tointeger(L, -1);
    lua_pop(L, 1);
    PR_DEBUG("val=%d",registerVal);
    
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d,channel=%d",fc,addr,cnt,slave,channel);

    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }
    
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_write_register(conn->pConnCxt,addr,registerVal)!= -1){
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
   
    return 0;

}
/*15 (0x0F) Write Multiple Coils*/
int __modbus_write_bits(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);

    uint8 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }
    lua_newtable(L);
    
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_write_bits(conn->pConnCxt,addr, cnt,rdData)!= -1){
    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    

    return 1;

}
/*16 (0x10) Write Multiple registers*/
int __modbus_write_registers(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-2)){
        vDBG_ERR("stack second is not table");
        return -1;
    }
    
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);
    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }

    if(!lua_istable(L,-2)){
        vDBG_ERR("stack -2 is not table");
        return -1;
    }

    uint16 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_write_registers(conn->pConnCxt,addr, cnt,rdData)!= -1){

    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    

    return 1;

}
/*17 (0x11) Report Server ID (Serial Line only)*/
int __modbus_report_slave_id(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);
    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }

    uint16 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }

    int max_dest;
    uint8_t *dest;
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_report_slave_id(conn->pConnCxt,max_dest,dest)!= -1){

    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    

    return 1;

}
/*22 (0x16) Mask Write Register*/
int __modbus_mask_write_register(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);
    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }

    uint16 and_mask,or_mask;

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    lua_newtable(L);
    
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_mask_write_register(conn->pConnCxt,addr,and_mask,or_mask)!= -1){

    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    

    return 1;

}
/*23 (0x17) Read/Write Multiple registers*/
int __modbus_write_and_read_registers(lua_State* L)
{
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    if(!lua_istable(L,-1)){
        vDBG_ERR("stack top is not table");
        return -1;
    }
    lua_pushstring(L,"fc");
    lua_gettable(L,-2);
    int fc=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"addr");
    lua_gettable(L,-2);
    int addr=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"cnt");
    lua_gettable(L,-2);
    int cnt=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"slave");
    lua_gettable(L,-2);
    int slave=lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L,"channel");
    lua_gettable(L,-2);
    int channel=lua_tointeger(L, -1);
    lua_pop(L, 1);

    vDBG_MODBUS(DBG_DEBUG,"fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);
    COMM_INFO_T*conn = user_get_conn_context_byChannel(channel);
    if(conn == NULL){
        vDBG_ERR("comm context is null");
        return 0;
    }

    uint16 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    int write_addr, write_nb,read_addr,read_nb;
    uint16_t *src,*dest;
  
    //01 (0x01) Read Coils
    modbus_set_slave(conn->pConnCxt, slave);
    if(ret = modbus_write_and_read_registers(conn->pConnCxt,write_addr,write_nb,src,read_addr,read_nb,dest)!= -1){

    }else{
        vDBG_ERR("errno=%d,faile=%s",errno,modbus_strerror(errno));
    }
    
    return 1;

}


/*
param1:devid
param2:dp
param3:dp count
param4:lua table index
*/
int send_report(lua_State* L){
    int ret;
    int n = lua_gettop(L);
    vDBG_INFO("stack param count=%d",n);
    int dp_idx=lua_tointeger(L, -1);
    
    lua_pushvalue(L,-2);
    int dp_cnt=lua_tointeger(L, -1);
    lua_pop(L,1);

    lua_pushvalue(L,-4);
    char devid[128]={0};
    strcpy((char *)&devid,lua_tostring(L,-1));
    lua_pop(L,1);
    
    vDBG_APP(DBG_DEBUG,"dp_cnt=%d,devid=%s,dp_idx=%d",dp_cnt,devid,dp_idx);
    if(dp_cnt<=0){
        lua_pushinteger(L,-1);
        return 1;
    }
    if(LUA_TTABLE!=lua_type(L, -3)){
        vDBG_ERR("stack -2 NOT TABLE !!! type=%s\r\n",lua_typename(L, lua_type(L, -3)));
        lua_pushinteger(L,-1);
        return 1;
    }
    TY_OBJ_DP_S*dp_send=(TY_OBJ_DP_S*)malloc(dp_cnt*sizeof(TY_OBJ_DP_S));
    if(dp_send==NULL){
        lua_pushinteger(L,-1);
        return 1;
    }
    lua_pushvalue(L,-3);//把dp table放到stack top
    for(int i=0;i<dp_cnt;i++){
        if(dp_cnt==1){
            lua_pushinteger(L,dp_idx);//lua array start index is 1            
        }else{
            lua_pushinteger(L,i+1);//lua array start index is 1
        }
        lua_gettable(L,-2);//第一个嵌套table放到stack top

        lua_pushstring(L,"dpid");
        lua_gettable(L, -2);
        dp_send[i].dpid=lua_tointeger(L,-1);
        lua_pop(L, 1);
        vDBG_MODBUS(DBG_DEBUG,"dp_send[%d].dpid=%d",i,dp_send[i].dpid);

        lua_pushstring(L,"type");
        lua_gettable(L, -2);
        dp_send[i].type=lua_tointeger(L,-1);
        lua_pop(L, 1);
        vDBG_MODBUS(DBG_DEBUG,"dp_send[%d].type=%d",i,dp_send[i].type);

        lua_pushstring(L,"value");
        lua_gettable(L, -2);
        switch(dp_send[i].type){
            case PROP_BOOL:
                dp_send[i].value.dp_bool=lua_tointeger(L,-1);
                vDBG_MODBUS(DBG_DEBUG,"dp_send[%d].value.dp_bool=%d",i,dp_send[i].value.dp_bool);
                break;
            case PROP_VALUE:
                dp_send[i].value.dp_value=lua_tointeger(L,-1);
                vDBG_MODBUS(DBG_DEBUG,"dp_send[%d].value.dp_value=%d",i,dp_send[i].value.dp_value);
                break;
            case PROP_STR:
                dp_send[i].value.dp_str=(char*)lua_tostring(L,-1);
                vDBG_MODBUS(DBG_DEBUG,"dp_send[%d].value.dp_str=%s",i,dp_send[i].value.dp_str);
                break;
            case PROP_ENUM:
                dp_send[i].value.dp_enum=lua_tointeger(L,-1);
                vDBG_MODBUS(DBG_DEBUG,"dp_send[%d].value.dp_enum=%d",i,dp_send[i].value.dp_enum);
                break;
            case PROP_BITMAP:
                dp_send[i].value.dp_bitmap=lua_tointeger(L,-1);
                vDBG_MODBUS(DBG_DEBUG,"dp_send[%d].value.dp_bitmap=%d",i,dp_send[i].value.dp_bitmap);
                break;
            default:
                break;
        }
        lua_pop(L, 2);//pop the vale and embed table
    }
    if((ret = dev_report_dp_json_async(devid,dp_send, dp_cnt))!=OPRT_OK){
        vDBG_ERR("ret=0x%04x",ret);
    }
    lua_pushinteger(L,ret);
    free(dp_send);
    return 1;
}

