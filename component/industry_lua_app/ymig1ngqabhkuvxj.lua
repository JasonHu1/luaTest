--[[
    Modbus 温湿度脚本
]]--
local math=require("math")

print("test jjjjjj")

attr={
    {
    fc=3,
    addr=0,
    cnt=2,
    dpid=1,
    type=1--温度
    },    
    {
    fc=31,
    addr=02,
    cnt=23,
    dpid=2,
    type=1--湿度
    }
}

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

function report_dp(source,target,channel,c)
    print("type(source)..",type(source))
    print("type(c)..",type(c))
    
    print("slaveAddr..",source)
    attribute.slave = source
    attribute.channel = channel
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

    dp[1]["value"]= nArray[0]--温度
    dp[2]["value"]= math.floor(nArray[1]/10)


    local ret=send_report(target,dp,attribute.cnt,0)
    if (ret == 0)then
        print("lua report ok")
    else
        print("lua report failed retCode=",ret)
    end
end

local attr_temp={
    fc=3,
    cnt=1,
    dpid=6,
    addr=4
}
function write_dp(source,target,channel,dptbl)
    print("type(source)..",type(source))
    print("type(c)..",type(dptbl))
    print("dptbl[1].dpid..",dptbl[1].dpid)
    print("dptbl[1].val..",dptbl[1].val)

    if (next(dptbl) ==nil) then
        print("modbus read occure error")
        return
    end

    for var=1,#dptbl+1,1 do
        if ( attr_temp.dpid == dptbl[var].dpid ) then
            print("slaveAddr..",source)
            attr_temp.slave = source
            attr_temp.channel = channel
            modbus_write_register(attr_temp,dptbl[var].val)
        else
            print("attr_temp.dpid",attr_temp.dpid, "dptbl.dpid=",dptbl[var].val)
        end
    end
end

