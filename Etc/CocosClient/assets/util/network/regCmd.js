let misc = require("misc");
let regCmd = {};

///////////////////注册自定义协议函数////////////////////
regCmd.cmdRange = [1025, 8000]; //指令范围
regCmd.cmdMapReq = {}; //请求指令MAP
regCmd.cmdMapRet = {}; //返回指令MAP

regCmd.cmdCheck = function (cmdNo, cmdName, proto) {
    misc.assert(cmdNo && cmdName && proto, "参数错误");
    misc.assert(cmdNo >= this.cmdRange[0] && cmdNo <= this.cmdRange[1], "非法指令号:"+cmdNo);
};

regCmd.regCmdReq = function (cmdNo, cmdName, proto, target) {
    target = target ? target : 0;
    this.cmdCheck(cmdNo, cmdName, proto);
    misc.assert(!this.cmdMapReq[cmdNo], "命令号重复注册"+cmdNo);
    misc.assert(!this.cmdMapReq[cmdName], "命令名重复注册"+cmdName);
    var cmd = [cmdNo, cmdName, proto, target];
    this.cmdMapReq[cmdNo] = cmd;
    this.cmdMapReq[cmdName] = cmd;
};

regCmd.regCmdRet = function (cmdNo, cmdName, proto, target) {
    target = target ? target : 0;
    this.cmdCheck(cmdNo, cmdName, proto);
    misc.assert(!this.cmdMapRet[cmdNo], "命令号重复注册"+cmdNo);
    misc.assert(!this.cmdMapRet[cmdName], "命令名重复注册"+cmdName);

    let module = null;
    if (cmdNo != 1100) {
        let game = require("game");
        module = game.user.findRpcHandler(cmdName);
        if (!module) {
            console.error("消息处理函数未定义:", cmdName);
            return;
        }
    }

    var cmd = [cmdNo, cmdName, proto, target, module];
    this.cmdMapRet[cmdNo] = cmd;
    this.cmdMapRet[cmdName] = cmd ;
};

regCmd.getCmdReq = function(sCmdName) {
    return this.cmdMapReq[sCmdName];
};

regCmd.getCmdRet = function(nCmdNo) {
    return this.cmdMapRet[nCmdNo];
};



///////////////////注册PROTOBUF协议函数////////////////////
regCmd.pbRange = [8001, 40000]; //指令范围
regCmd.pbMapReq = {}; //请求指令MAP
regCmd.pbMapRet = {}; //返回指令MAP

regCmd.pbCmdCheck = function (cmdNo, cmdName, proto) {
    misc.assert(cmdNo && cmdName && proto, "参数错误");
    misc.assert(cmdNo >= this.pbRange[0] && cmdNo <= this.pbRange[1], "非法指令号:"+cmdNo);
};

regCmd.regPBReq = function (cmdNo, cmdName, proto, target) {
    target = target ? target : 0;
    this.pbCmdCheck(cmdNo, cmdName, proto);
    misc.assert(!this.pbMapReq[cmdNo], "命令号重复注册"+cmdNo);
    misc.assert(!this.pbMapReq[cmdName], "命令名重复注册"+cmdName);
    var cmd = [cmdNo, cmdName, proto, target];
    this.pbMapReq[cmdNo] = cmd;
    this.pbMapReq[cmdName] = cmd;
};

regCmd.regPBRet = function (cmdNo, cmdName, proto, target) {
    target = target ? target : 0;
    this.pbCmdCheck(cmdNo, cmdName, proto);
    misc.assert(!this.pbMapRet[cmdNo], "命令号重复注册"+cmdNo);
    misc.assert(!this.pbMapRet[cmdName], "命令名重复注册"+cmdName);

    let game = require("game");
    module = game.user.findRpcHandler(cmdName);
    if (!module) {
        console.error("消息处理函数未定义:", cmdName);
        return;
    }

    var cmd = [cmdNo, cmdName, proto, target, module];
    this.pbMapRet[cmdNo] = cmd;
    this.pbMapRet[cmdName] = cmd;
};

regCmd.getPBReq = function(sCmdName) {
    return this.pbMapReq[sCmdName];
};

regCmd.getPBRet = function(nCmdNo) {
    return this.pbMapRet[nCmdNo];
};





module.exports = regCmd 