--[[
    Modbus 一氧化碳
发送数据:
03 03 00 06 00 01 65 E9 
接收数据:
03 03 02 00 00 C1 84 
解析数据:
一氧化碳浓度:0ppm 
]]--
local math=require("math")


local attribute={
    {
    fc=3,
    addr=6,
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

function Sleep1(n)
    os.execute("sleep " .. n)
end

print("ENUM_TYPE.PROP_BOOL",ENUM_TYPE.PROP_BOOL)
print("ENUM_TYPE.PROP_VALUE",ENUM_TYPE.PROP_VALUE)
print("ENUM_TYPE.PROP_VALUE",ENUM_TYPE.PROP_VALUE)
print("ENUM_TYPE.PROP_ENUM",ENUM_TYPE.PROP_ENUM)
print("ENUM_TYPE.PROP_BITMAP",ENUM_TYPE.PROP_BITMAP)

local dp={
    {dpid=2,type=1},--CO 一氧化碳
}

function report_dp(source,target)
    print("type(source)..",type(source))
    print("slaveAddr..",source)
    for i=1,1,1 do
        attribute[i].slave = source
        print("type(attribute[i])..",type(attribute[i]))
        print("attribute[i].addr..",attribute[i].addr)
        local nArray = modbus_read_registers(attribute[i])

        print("type(nArray)..",type(nArray))
        print("nArray=",nArray)
        print("length=",#nArray)
        for n=0,#nArray,1 do
            print("nArray=",nArray[n]) 
        end

        print("dp[n].dpid=",dp[i].dpid) 
        dp[i]["value"]= nArray[0]
        print("dp[n+1][\"value\"] = ",dp[i]["value"])

        local ret=send_report(target,dp,attribute[i].cnt,i)
        if (ret == 0)then
            print("lua report ok")
        else
            print("lua report failed retCode=",ret)
        end
        Sleep1(2)
    end
end

