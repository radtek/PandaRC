// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html
let misc = require("misc");
let comModuleBase = require("moduleBase");

cc.Class({
    extends: comModuleBase,
    
    properties: {
    },

    //自定义消息分发
    dispatchCmdMsg(cmdNo, cmdName, source, module, data) {
        let fnHandler = module[cmdName];
        if (!fnHandler) {
            console.error("消息处理函数未定义:", cmdName);
            return; 
        }
        fnHandler.call(module, data);
    },

    //proto消息分发
    dispatchPBMsg(cmdNo, cmdName, source, module, data) {
        this.dispatchCmdMsg(cmdNo, cmdName, source, module, data);
    },

    //查找网络消息处理函数
    findRpcHandler(cmdName) {
        if (this[cmdName]) {
            return this; 
        }
        for (let k in this.moduleMap) {
            let module = this.moduleMap[k];
            if (module[cmdName]) {
                return module;
            }
        }
    },

    ctor() {
        this.moduleMap = {};
    },

    onRelease() {
    },

    initModule() {
    },

    TestPack(data) {

    },
    LoginRet(data) {

    },
});
