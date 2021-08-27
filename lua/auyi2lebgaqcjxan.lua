--[[
    Modbus 二氧化碳
]]--
local math=require("math")

print("test jjjjjj")
local attribute={
    fc=3,
    addr=0,
    cnt=2,
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
print("ENUM_TYPE.PROP_VALUE",ENUM_TYPE.PROP_VALUE)
print("ENUM_TYPE.PROP_VALUE",ENUM_TYPE.PROP_VALUE)
print("ENUM_TYPE.PROP_ENUM",ENUM_TYPE.PROP_ENUM)
print("ENUM_TYPE.PROP_BITMAP",ENUM_TYPE.PROP_BITMAP)

local devid="0006dffffe67e2ac"
function report_dp(source,target)
    print("type(source)..",type(source))
    print("slaveAddr..",source)
    attribute.slave = source
    local nArray = modbus_read_registers(attribute)

    print("type(nArray)..",type(nArray))
    if (next(nArray) ==nil) then
        print("modbus read occure error")
        return
    end
    print("nArray=",nArray)
    print("length=",#nArray)
    for n=0,#nArray,1 do
       print("nArray=",nArray[n]) 
    end

    local dp={
        {dpid=1,type=1},--温度
        {dpid=2,type=1}--湿度
    }
    for n=0,#nArray,1 do
        print("dp[n].dpid=",dp[n+1].dpid) 
        dp[n+1]["value"]= math.floor(nArray[n]/10)
        print("dp[n+1][\"value\"] = ",dp[n+1]["value"])
     end


    local ret=send_report(target,dp,attribute.cnt)
    if (ret == 0)then
        print("lua report ok")
    else
        print("lua report failed retCode=",ret)
    end
end

