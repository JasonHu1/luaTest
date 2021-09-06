#ifndef __LUA_MODBUS_H__
#define __LUA_MODBUS_H__
#include "modbus.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#ifdef __cplusplus
}
#endif


int __modbus_read_bits(lua_State* L);
int __modbus_read_input_bits(lua_State* L);
int __modbus_read_registers(lua_State* L);
int __modbus_read_input_registers(lua_State* L);
int __modbus_write_bit(lua_State* L);
int __modbus_write_register(lua_State* L);
int __modbus_write_bits(lua_State* L);
int __modbus_write_registers(lua_State* L);
int __modbus_report_slave_id(lua_State* L);
int __modbus_mask_write_register(lua_State* L);
int __modbus_write_and_read_registers(lua_State* L);
int send_report(lua_State* L);
int app_modbus_read_registers(char*devid,int addr,int dpid,int type,int slave,int channel);
int app_modbus_write_register(char*devid,int addr,int val,int slave,int channel);


#endif//__LUA_MODBUS_H__

