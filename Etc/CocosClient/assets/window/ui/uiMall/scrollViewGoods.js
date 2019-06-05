// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html

let comScrollViewBase = require("scrollViewBase");

cc.Class({
    extends: comScrollViewBase,

    properties: {
    },

    // LIFE-CYCLE CALLBACKS:

    onLoad () {
        this._super();
        this.content.on("touchend", function(touches, event) {
            let location = touches.getLocation();
            for (let k = 0; k < this.titlesList.length; k++) {
               let title = this.titlesList[k];
               let titleRect = title.getBoundingBoxToWorld();
               if (titleRect.contains(location)) {
                    console.log("hit--->", k);
               }
            }
        }, this);
    },

    start () {
    },

    // update (dt) {},
});
