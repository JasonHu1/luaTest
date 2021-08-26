#!/usr/local/bin/lua
print("test jjjjjj")

local attribute={
    fc=3,
    addr=4,
    cnt=5,
    6
}

local ENUM_TYPE={
    PROP_BOOL =0,
    PROP_VALUE =1,
    PROP_STR =2,
    PROP_ENUM =3,
    PROP_BITMAP =4
}

print("ENUM_TYPE.PROP_BOOL",ENUM_TYPE.PROP_BOOL)
print("ENUM_TYPE.PROP_BOOL",ENUM_TYPE.PROP_VALUE)
print("ENUM_TYPE.PROP_BOOL",ENUM_TYPE.PROP_STR)
print("ENUM_TYPE.PROP_BOOL",ENUM_TYPE.PROP_ENUM)
print("ENUM_TYPE.PROP_BOOL",ENUM_TYPE.PROP_BITMAP)

local devid="0006dffffe67e2ac"
function report_dp(source,target,c)
    print("type(source)..",type(source))
    print("type(c)..",type(c))

    print("c.dpid..",(c.dpid))
    print("c.type..",c.type)

    print("slaveAddr..",source)

    attribute.slave = source
    local nArray = modbus_read_bits(attribute)

    print("type(nArray)..",type(nArray))
    print("nArray=",nArray)
    print("length=",#nArray)
    for n=0,#nArray,1 do
       print("nArray=",nArray[n]) 
    end

    local dp={
        {dpid=1,type=1},
        {dpid=2,type=1},
        {dpid=3,type=1},
        {dpid=4,type=1},
        {dpid=5,type=1},
        {dpid=6,type=1}
    }
    for n=1,#nArray,1 do
        print("dp[n].dpid=",dp[n].dpid) 
        dp[n]["value"]=nArray[n]
     end


    local ret=send_report(devid,dp,6)
    if (ret == 0)then
        print("lua report ok")
    else
        print("lua report failed retCode=",ret)
    end
end


gloval_upvalue = 10
gloval_upvalue1 = 20
local local_upvalue = 100
 
function l_counter()
   return function ()
      local_upvalue = local_upvalue + 1
      return local_upvalue
   end
end
 
function g_counter()
   return function ()
      gloval_upvalue = gloval_upvalue + 1
      return gloval_upvalue,gloval_upvalue1
   end
end
 
g_testf = g_counter()
l_testf = l_counter()
 
function gtest()
  print(g_testf())
end
 
 
function ltest()
  print(l_testf())
end
 
upvalue_test(1,2,3)
upvalue_test(4,5,6)
