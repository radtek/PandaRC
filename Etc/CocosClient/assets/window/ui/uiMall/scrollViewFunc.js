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
let comScrollViewBase = require("scrollViewBase");

cc.Class({
    extends: comScrollViewBase,

    properties: {
    },

    // LIFE-CYCLE CALLBACKS:

    onLoad () {
        this._super();
        this._touchTitle = null;
        this.content.on("touchstart", function(touches, event) {
            let location = touches.getLocation();
            for (let k = 0; k < this.titlesList.length; k++) {
               let title = this.titlesList[k];
               let titleRect = title.getBoundingBoxToWorld();
               if (titleRect.contains(location)) {
                    this.onTitleTouchStart(k, title);
               }
            }
        }, this);
        this.content.on("touchend", function(touches, event) {
            let location = touches.getLocation();
            for (let k = 0; k < this.titlesList.length; k++) {
               let title = this.titlesList[k];
               let titleRect = title.getBoundingBoxToWorld();
               if (titleRect.contains(location)) {
                    this.onTitleTouchEnd(k, title);
               }
            }
        }, this);

        this.content.on("touchcancel", function(touches, event) {
            this.onTitleTouchCancel();
        }, this);
    },

    onTitleTouchStart(k, title) {
        let node = title.getChildByName("panelFunc");
        node.color = new cc.Color(167, 167, 167);
        this._touchTitle = title;
    },

    onTitleTouchEnd(k, title) {
        if (this._touchTitle == title) {
            let node = title.getChildByName("panelFunc");
            node.color = new cc.Color(255, 255, 255);
            this._touchTitle = null;
        }
        game.uiMgr.createUIFromPath("panelMessage", "prefab/prefabMall/panelMessage");
    },

    onTitleTouchCancel() {
        console.log("touch cancel****");
        if (this._touchTitle) {
            let node = this._touchTitle.getChildByName("panelFunc");
            node.color = new cc.Color(255, 255, 255);
            this._touchTitle = null
        }

    },

    start () {
    },

    // update (dt) {},
});
