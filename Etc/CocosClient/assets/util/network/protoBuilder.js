var protoBuf = require("protobuf"); 
var byteBuffer = require("bytebuffer");  
var protoFiles = require("protoFiles");

protoBuf.Util.fetchOut = function (importFilename)
{
    var jsFileName = importFilename.split(".")[1].substr(1);
    return protoFiles[jsFileName];
};

//builder列表
var builderMap = {};
var loadProto = function () {
    for (var id in protoFiles) {
        var cont = protoFiles[id];
        console.log("loading proto***", id);
        var builder = protoBuf.protoFromString(cont, null, id);
        var packName = builder.ptr.children[builder.ptr.children.length-1].name;
        builderMap[packName] = builder;
    }
};
var build = function(proto) {
    var packName = proto.split(".")[0];
    var builder = builderMap[packName];
    var cMsg = builder.build(proto);  
    return cMsg;
};


//export proto builder
var pbd = {}
pbd.decode = function (data, proto) {
    var cMsg = this.build(proto);
    return cMsg.decode(data);
};

pbd.message = function (proto) {
    var cMsg = this.build(proto);  
    var msg = new cMsg();
    return msg;
};

//测试
pbd.testProto = function () {
    //encode
    var msg = this.message("login.LoginReq");
    msg.set("sAccount","webTest");  
    msg.set("sPassword","123456");
    msg.set("sPlatform","ios");
    msg.set("sChannel","1000");
    var buffer = msg.encode().toBuffer();
    console.log("encode:", buffer, buffer.byteLength);

    //decode
    var dcmsg = this.decode(buffer, "login.LoginReq");  
    console.log("decode:", dcmsg)
};

module.exports = pbd;