--[[
    Modbus PM2.5/10
发送数据:
01 03 00 04 00 01 C5 CB   
01 03 00 09 00 01 54 08 
接收数据:
01 03 02 00 17 F8 4A   
01 03 02 00 21 78 5C 
解析数据:
pm2.5浓度:23ug/m3  pm10浓度:33ug/m3  
]]--
local math=require("math")


local attribute={
    attr_pm2_5={
        fc=3,
        addr=4,
        cnt=1
    },attr_pm10={
        fc=3,
        addr=9,
        cnt=1
    }
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


local dp={
    {dpid=2,type=1},--pm2.5
    {dpid=24,type=1}--pm10
}

function report_dp(source,target)
    print("type(source)..",type(source))
    print("slaveAddr..",source)
    for i=1,2,1 do
        attribute[i].slave = source
        local nArray = modbus_read_registers(attribute[i])

        print("type(nArray)..",type(nArray))
        print("nArray=",nArray)
        print("length=",#nArray)
        for n=0,#nArray,1 do
        print("nArray=",nArray[n]) 
        end

        print("dp[n].dpid=",dp[i].dpid) 
        dp[i]["value"]= nArray[0]*256+nArray[1]
        print("dp[n+1][\"value\"] = ",dp[i]["value"])

        local ret=send_report(target,dp,attribute[i].cnt)
        if (ret == 0)then
            print("lua report ok")
        else
            print("lua report failed retCode=",ret)
        end
    end
end

