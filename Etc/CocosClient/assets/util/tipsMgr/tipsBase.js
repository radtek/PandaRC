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
    },

    ctor() {
        //这里还没有场景且不能loadRes
        this.initState = 0; //0未初始化; 1初始化中; 2初始化完成

        this.tipsList = [];
        this.cacheNodes = 20;
        this.tipsPoolName = "";
        this.tipsContainer = new cc.Node();

        this.speed = 600;       //移动速度(像素每秒)
        this.interval = 0.03;    //计时器间隔(秒)
        this.holdTime = 1000;   //悬停静止时间(毫秒)

        this.callbackList = []; //缓存初始化过程中的调用的回调
    },

    _init(fnCallback) {
        if (this.initState == 2) {
            fnCallback()
            return;

        } else if (this.initState == 0) {
            let self = this;
            this.initState = 1;
            this.callbackList.push(fnCallback);

            this._initContainer();
            this._initTipsPrefab(function() {
                self.initState = 2;
                for (let k=0; k<self.callbackList.length; k++) {
                    self.callbackList[k]();
                }
                self.callbackList = [];
            });

        } else if (this.initState == 1) {
            this.callbackList.push(fnCallback);

        }
    },

    _initContainer() {
        console.error("请在子类实现");
    },

    _initTipsPrefab() {
        console.error("请在子类实现");
    },

    //将节点添加到容器(底部添加)
    _addContainer(node) {
        node.opacity = 255;
        let offsetHeight = this.tipsContainer.height/2;

        for (let i = this.tipsList.length-1; i >= 0; i--) {
            let item = this.tipsList[i];
            let tarPosY = (item[1].height*(i+1))+item[1].height/2 - offsetHeight;
            if (item[1].y < tarPosY) {
                item[1].y = tarPosY;
            }
         }
        node.y = node.height/2 - offsetHeight;
        node.parent = this.tipsContainer;
        this.tipsList.unshift([misc.msTime(), node, node.y]);

        this.tipsContainer.parent = cc.director.getScene();
        this.tipsContainer.zIndex = 1024; //永远在前面

        if (this.tipsList.length == 1) {
            this.schedule(this._onTimer, this.interval);
        }
   },

   //定时器控制动画
   _onTimer() {
        let game = require("game");
        let offsetHeight = this.tipsContainer.height/2;
        let msTime = misc.msTime();
        let sliceIndex = -1;

        for (let i = this.tipsList.length-1; i >= 0; i--) {
            let item = this.tipsList[i];
            if (msTime - item[0] < this.holdTime) {
                break;
            }
            // item[1].y += this.interval * this.speed;
            // let distance = item[1].y + offsetHeight;

            item[2] += this.interval * this.speed;
            let distance = item[2] + offsetHeight;
            let distRatio = Math.max(0, 1 - distance/this.tipsContainer.height);

            item[1].opacity = 255 * distRatio;
            if (item[2] >= offsetHeight+item[1].height) {
                game.npMgr.put(this.tipsPoolName, item[1]);
                sliceIndex = i;
            }
        }
        if (sliceIndex >= 0) {
            this.tipsList = this.tipsList.slice(0, sliceIndex);
        }
        if (this.tipsList.length == 0) {
            this.tipsContainer.parent = null;
            this.unschedule(this._onTimer);
        }
   },

    tips(cont) {
        console.error("请在子类实现");
    },

    //关闭所有TIPS
    closeAllTips() {
        let game = require("game");
        for (let i = this.tipsList.length-1; i >= 0; i--) {
            let item = this.tipsList[i];
            game.npMgr.put(this.tipsPoolName, item[1]);
        }
        this.tipsList = [];
        this.tipsContainer.parent = null;
        this.unschedule(this._onTimer);
    }

    // update (dt) {},
});
