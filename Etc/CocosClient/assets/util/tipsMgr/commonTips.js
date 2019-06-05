// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html

let comTipsBase = require("tipsBase");

cc.Class({
    extends: comTipsBase,

    properties: {
    },

    ctor() {
        this.tipsPoolName = "commonTips"; //对象池
    },

    //初始化容器
    _initContainer() {
        let scene = cc.director.getScene();
        let canvas = scene.getChildByName("Canvas");
        this.tipsContainer.width = 1;
        this.tipsContainer.height = canvas.height/2;
        this.tipsContainer.x = canvas.width/2;
        this.tipsContainer.y = canvas.height/4*3;
    },

    //初始化TIPS资源
    _initTipsPrefab(fnCallback) {
        let self = this;
        let game = require("game");
        cc.loader.loadRes("prefab/panelCommonTips", function (err, prefab) {
            if (!err) {
                for (let i = 0; i < self.cacheNodes; i++) {
                    var node = cc.instantiate(prefab);
                    game.npMgr.put(self.tipsPoolName, node);
                }
                fnCallback();

            } else {
                console.error(err);

            }

        });
    },

    //创建TIPS对象
    _createTips(cont) {
        let game = require("game");
        let node = game.npMgr.get(this.tipsPoolName);
        if (!node) {
            console.error(this.tipsPoolName, "is empty!");
            return 
        }
        let richNode = node.getChildByName("richtext")
        let richText = richNode.getComponent(cc.RichText);
        richText.string = cont;
        return node;
    },

    tips(cont) {
        let self = this;
        let game = require("game");
        this._init(function() {
            let node = self._createTips(cont);
            if (!node) {
                return; 
            }
            self._addContainer(node);
        });
    },
});
