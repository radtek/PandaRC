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

cc.Class({
    extends: cc.Component,

    properties: {
        btnHome: cc.Button,
        btnMy: cc.Button,
    },

    // LIFE-CYCLE CALLBACKS:

    // onLoad () {},

    start () {
        this.btnHome.node.on("click", function(event) { this.onBtnHome(); }, this)
        this.btnMy.node.on("click", function(event) { this.onBtnMy(); }, this)
    },

    // update (dt) {},

    onBtnHome: function() {
        game.sceneMgr.preloadScene("sceneHome");
    },

    onBtnMy: function() {
        game.sceneMgr.preloadScene("sceneMy");
    },
});
