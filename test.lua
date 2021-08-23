
function encode_test(source,target)
    print("------in lua parse--------")
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
    "fc"=03,
    "addr"=04,
    "cnt"=5
}
local devid="0006dffffe67e2ac"
function report_dp(source,target)
    print("type(source)..",type(source))
    local resultArray = readData(attribute)

    local dp={"dpid"=2,"type"=2}
    dp["value"]=resultArray[0]

    local ret=send_report(devid,dp,1)
    if (ret == 0)then
        print("lua report ok")
    else
        print("lua report failed retCode=",ret)
    end
end

