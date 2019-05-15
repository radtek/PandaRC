//所有不在场景里面的组件的统一控制脚本

// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html

let comUser = require("user");
let comNpMgr = require("npMgr");
let comTcpNet = require("tcpNet");
let comHttpNet = require("httpNet");
let comUIMgr = require("uiMgr");
let comSceneMgr = require("sceneMgr");
let comTipsMgr = require("tipsMgr");

let comGame = cc.Class({
    extends: cc.Component,

    properties: {
    },

    ctor() {
        this.user = new comUser();
        this.npMgr = new comNpMgr();
        this.tcpNet = new comTcpNet();
        this.httpNet = new comHttpNet();
        this.uiMgr = new comUIMgr();
        this.sceneMgr = new comSceneMgr();
        this.tipsMgr = new comTipsMgr();
        
    },
});




module.exports = new comGame();
