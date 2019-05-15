// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html

//一级界面应该放到此管理器里面,避免频繁释放创建导致卡顿

let misc = require("misc");
let comUIBase = require("uiBase");

cc.Class({
    extends: cc.Component,

    properties: {
        uiMap: {default: {},},
    },

    ctor() {
        this.oldParent = null; //用于判断界面加载中场景是否已被切换
    },

    //取场景显示器
    getSceneViewer() {
        // return cc.director.getScene().getChildByName("sceneViewer");
        return cc.director.getScene().getChildByName("Canvas");
    },

    //@name 界面名称
    //@prefab 界面预制件
    //@是否返回节点,如果是false,则直接添加进场景
    createUIFromPrefab(name, prefab=null, returnNode=false) {
        //设置场景自动释放情况下,不自动释放prefab
        cc.loader.setAutoReleaseRecursively(prefab, false);
        this.oldParent = this.getSceneViewer();

        let node = this.uiMap[name];
        if (!node) {
            node = cc.instantiate(prefab);
            misc.assert(node, "实例化prefab["+prefab+"]失败");
            this.uiMap[name] = node;
        }
        misc.assert(cc.isValid(node), "UI节点["+name+"]已被销毁但没有显式调用uiMgr.destroyUI(通常是切换场景时随场景一起销毁了)");

        //调用基类的onLoad,界面名称==挂载的脚本名称
        let scriptComponent = node.getComponent(name);
        if (scriptComponent instanceof comUIBase) {
            comUIBase.prototype.onLoad.call(scriptComponent);
        }
        if (returnNode) {
            return node;
        }
        if (this.oldParent == this.getSceneViewer()) {
            node.parent = this.oldParent;
        }
    },

    //@name 界面名称
    //@prefabPath 预制件资源路径
    //@fnCallback 创建完成回调,传null则直接添加进场景
    //@showLoading 是否显示加载动画
    createUIFromPath(name, prefabPath, fnCallback = null, showLoading=true) {
        let self = this;
        this.oldParent = this.getSceneViewer();

        let node = this.uiMap[name];
        if (node) {
            misc.assert(cc.isValid(node), "UI节点["+name+"]已被销毁但没有显式调用uiMgr.destroyUI(通常是切换场景时随场景一起销毁了)");
            if (fnCallback) {
                fnCallback(node);
                return;
            }
            if (this.oldParent == this.getSceneViewer()) {
                node.parent = this.oldParent;
            }
            return;
        }

        let fnAfterLoading = function() {
            cc.loader.loadRes(prefabPath, function (err, prefab) {
                self._colseLoading();
                if (err) {
                    console.error(err);
                    return;
                } 
                let returnNode = fnCallback;
                let node = self.createUIFromPrefab(name, prefab, returnNode);
                if (returnNode) {
                    fnCallback(node);
                }
            });
        }

        if (name == "panelLoading") {
           fnAfterLoading(); 

        } else {
            if (showLoading) {
               this._displayLoading(fnAfterLoading);
            } else {
                fnAfterLoading();
            }

        }
    },

    //显示加载状态
    _displayLoading(fnCallback) {
        this.createUIFromPath("panelLoading", "prefab/panelLoading", function(node) {
            node.parent = cc.director.getScene();
            fnCallback();
        })
    },

    //关闭加载状态
    _colseLoading() {
        this.closeUI("panelLoading");
    },

    //UI名称做参数
    closeUI(name, cleanup=false) {
        let node = this.uiMap[name];
        if (!node) {
            return;
        }
        node.removeFromParent(cleanup);
    },

    //UI节点做参数
    closeUINode(node, cleanup=false) {
        node.removeFromParent(cleanup);
    },

    //销毁特定界面(并没有释放任何贴图)
    destroyUI(name) {
        let node = this.uiMap[name]
        if (!node) {
            return;
        }
        node.destroy();
        this.uiMap[name] = null;
    },

    //关闭所有UI
    closeAllUI() {
        for (let name in this.uiMap) {
            this.closeUI(name);
        }
    },

    //打开设置了跨场景保留的界面
    openKeepOpenUI() {
        for (let name in this.uiMap) {
            let node = this.uiMap[name];
            if (node.getComponent(name).isKeepOpen()) {
                this.createUIFromPrefab(name);
            }
        }
    },

    //销毁所有界面
    destroyAllUI() {
        for (let name in this.uiMap) {
            this.destroyUI(name);
        }
    },
});
