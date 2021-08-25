#include "lua_modbus.h"
#include "subdev.h"

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

    vdbg_printf("fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);

    uint8 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    //01 (0x01) Read Coils
    modbus_set_slave(ctx[FD_RANK_SERIAL_START], slave);
    if(ret = modbus_read_bits(ctx[FD_RANK_SERIAL_START],addr, cnt,rdData)!= -1){
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

    vdbg_printf("fc=%d,addr=%d,cnt=%d",fc,addr,cnt)    ;

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

    vdbg_printf("fc=%d,addr=%d,cnt=%d,slave=%d",fc,addr,cnt,slave);

    uint16 rdData[32]={0};

    if(cnt>32){
        vDBG_WARN("static apply memery max is 32 Bytes,BUT cnt=%d",cnt);
        return 0;
    }
    //01 (0x01) Read Coils
    modbus_set_slave(ctx[FD_RANK_SERIAL_START], slave);
    if(ret = modbus_read_registers(ctx[FD_RANK_SERIAL_START],addr, cnt,rdData)!= -1){
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
    for(int i=0;i<cnt;i++){
        lua_pushinteger(L,i);//key
        lua_pushinteger(L,rdData[i]);
        lua_settable(L,-3);
    }

    return 1;

}
int __modbus_read_input_registers(lua_State* L){}
int __modbus_write_bit(lua_State* L){}
int __modbus_write_register(lua_State* L){}

int __modbus_write_bits(lua_State* L){}
int __modbus_write_registers(lua_State* L){}
int __modbus_report_slave_id(lua_State* L){}

int __modbus_mask_write_register(lua_State* L){}
int __modbus_write_and_read_registers(lua_State* L)
{}
