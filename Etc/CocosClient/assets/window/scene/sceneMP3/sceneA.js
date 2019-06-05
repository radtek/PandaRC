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
let misc = require("misc");
let comSceneBase = require("sceneBase");

cc.Class({
    extends: comSceneBase,

    properties: {
        btnDialog: cc.Button,
    },

    // LIFE-CYCLE CALLBACKS:
    ctor() {
    },

    onLoad () {
        let node = cc.find("nodePersist").getComponent("persistNode");
        console.error("sceneA***", node.data);

        this.node.on("click", function() {
            let game = require("game");
            game.user.sendEvent("sceneViewer/panelTest", "say-hello", "---say hello---");

            game.sceneMgr.preloadScene("B");
        }, this);

        this.btnDialog.node.on("click", function() {
            game.uiMgr.createUIFromPath("panelTest", "prefab/panelTest");
            game.tipsMgr.commonTips("游戏游戏游戏");

        }, this);

        misc.loadConf("serverUrl.json", function(data) {
            console.log(data, "****###");
        });

    },

    start () {
    },

    // update (dt) {},
});
