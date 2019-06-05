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

cc.Class({
    extends: cc.Component,

    properties: {
        _keepOpen: false,  //切换场景是否保持打开状态
    },

    isKeepOpen() {
        return this._keepOpen;
    }, 

    setKeepOpen(keepOpen) {
       this._keepOpen = keepOpen; 
    },

    onLoad() {
        let self = this;
        //屏蔽穿透
        this.node.on('touchstart', function(event) {
            event.stopPropagation();
        });

        //关闭事件监听
        let btnClose = cc.find("content/btnClose", this.node) 
        if (btnClose) {
           btnClose.on("touchend", function(event) {
                if (self.onPanelClose) {
                    self.onPanelClose();
                }
                let game = require("game");
                game.uiMgr.closeUINode(this.node);
           }, this);
       }

    },

});
