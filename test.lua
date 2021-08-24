
print("test jjjjjj")
local attribute={
    fc=3,
    addr=4,
    cnt=5,
    6
}

local attribute1={
    fc="3",
    addr="4",
    cnt="5",
    "6"
}

local devid="0006dffffe67e2ac"
function report_dp(source,target)
    print("type(source)..",type(source))
    print("sum..",source + target)
    local nArray = readData(attribute)

    print("type(nArray)..",type(nArray))
    print("nArray=",nArray)
    print("length=",#nArray)
    for n=0,#nArray,1 do
       print("nArray=",nArray[n]) 
    end

    print("jason-------")
    --local dp={dpid=2,type=2}
    --dp["value"]=resultArray[0]

    --local ret=send_report(devid,dp,1)
    --if (ret == 0)then
        print("lua report ok")
    --else
        print("lua report failed retCode=",ret)
    --end
end

