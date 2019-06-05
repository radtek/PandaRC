// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html

cc.Class({
    extends: cc.Component,

    properties: {
    },

    ctor() {
    },

    //直接跳转,没有进度条
    loadScene(sceneName, fnLaunched) {
        let curScene = cc.director.getScene();

        let game = require("game");
        game.uiMgr.closeAllUI();
        game.tipsMgr.closeAllTips();

        cc.director.loadScene(sceneName, function() {
            if (fnLaunched) {
                fnLaunched();
            }
            game.uiMgr.openKeepOpenUI();
        }); 

    },

    //预加载进度
    _onProgress(completedCount, totalCount, latestItem) {
        console.log("onProgress***", completedCount, totalCount, latestItem);
    },

    //预加载完成
    _onLoaded(error, asset, sceneName) {
        if (error) {
            console.error(error);
            return;  
        }
        this.loadScene(sceneName);
    },

    //显示加载状态
    _displayLoading() {

    },

    //关闭加载状态
    _closeLoading() {

    },

    //预加载,带进度条
    preloadScene(sceneName) {
        let self = this;
        let curScene = cc.director.getScene();

        //加载进度
        cc.loader.onProgress = function(completedCount, totalCount, latestItem) {
            self._onProgress(completedCount, totalCount, latestItem); 
        }

        this._displayLoading();
        cc.director.preloadScene(sceneName, function(error, asset) {
            self._closeLoading();
            cc.loader.onProgress = null;
            self._onLoaded(error, asset, sceneName);
        });
    }
});
