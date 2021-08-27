--[[
    Modbus 温湿度脚本
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

function report_dp(source,target,c)
    print("type(source)..",type(source))
    print("type(c)..",type(c))
    
    print("slaveAddr..",source)
    attribute.slave = source
    local nArray = modbus_read_registers(attribute)

    print("type(nArray)..",type(nArray))
    print("nArray=",nArray)
    print("length=",#nArray)
    for n=0,#nArray,1 do
       print("nArray=",nArray[n]) 
    end

    local dp={
        {dpid=1,type=1},--温度
        {dpid=2,type=1}--湿度
    }

    dp[1]["value"]= nArray[0]--温度
    dp[2]["value"]= math.floor(nArray[1]/10)


    local ret=send_report(target,dp,attribute.cnt,0)
    if (ret == 0)then
        print("lua report ok")
    else
        print("lua report failed retCode=",ret)
    end
end

