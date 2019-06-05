// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html

let game = require("game");
let comUIBase = require("uiBase");

cc.Class({
    extends: comUIBase,

    properties: {
        btnClose: cc.Node,
    },

    ctor() {
    },

    //因为有缓存,只会触发1次
    onLoad () {
        this.node.on('say-hello', function(event) {
            console.log(event.detail);

        }, this);
    },

    //每次打开都会触发
    onEnable () {
        console.log("onEnable****");
    },

    //关闭界面事件
    onPanelClose() {
        console.log("panelTest onPanelClose***");
    },
});
