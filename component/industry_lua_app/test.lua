print("test jjjjjj")

function encode_test(source,target)
    print("------in lua parse--------")
    print("type(source)...",type(source))
    print("type(source.m_public)...",type(source.m_public))
    print("type(target)...",type(target))
    
    print("source.m_public",source.m_public)
    source.m_public = source.m_public+1
    print("source.cmd_tp",source.cmd_tp)
    source.cmd_tp = source.cmd_tp+1
    print("source.get_dps_cnt",source.get_dps_cnt)
    
    -- create a class instance
    local a = test.A('moo')
    a:printName()

    -- create a class instance
    local t = test.ThirdFormat()
    t:setpayload("set info to C++")
    t.retVal=5
    t:printpayload();
    print("t.retVal=",t.retVal)
    
    --target=t 不能这样赋值给C++的指针内存

    target.retVal=6
    target.setpayload(target,"set info to C++ 11")
    target:setname("set info to C++ 12")

end

function report_dp1(source,targ)
    local dpNode=test.TuyaProtoElement()
    dpNode.dpid = 1
    dpNode.type = 2
    dpNode.valuestr="hello lua"
    dpNode:send()
end

function report_dp2(source,targ)
    print("type(source)..",type(source))
    local dpNode={}
    print(type(dpNode))
    for i=0,2,1 do
        dpNode[i]=test.TuyaProtoElement()
        print(type(dpNode[i]))
        dpNode[i].dpid = i
        dpNode[i].type = i
        dpNode[i].valuestr="hello lua"
        print("dpNode[i] ",dpNode[i].dpid)
    end
    print(type(dpNode))
    test.sendPtr(dpNode)
end
function report_dp3(source,targ)
    print("type(source)..",type(source))
    local dpNode=test.TuyaProtoElement()
    print(type(dpNode))
    dpNode:create_DP_Array(3)
    for i=0,2,1 do
        print(type(dpNode.pDpArray[i]))
        --dd = test.TY_OBJ_DP_S()
        --dd.dpid = i
        dpNode.pDpArray[i].dpid = i
        print("dpNode[i] ",dpNode.pDpArray[i].dpid)
    end
    print(type(dpNode))
    dpNode:sendPtr()
end

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

