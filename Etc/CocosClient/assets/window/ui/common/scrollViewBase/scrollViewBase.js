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

    // LIFE-CYCLE CALLBACKS:

    onLoad() {
        this.isInit = false;

        this._rowSpacing = 10,    //行距(像素)
        this._itemSpacing = 5,    //物品间距 
        this._pageSize = 20,      //行/页
        this._pageIndex = 1,      //页码
        this._pageCount = 1,      //页数

        this.updateTimer = 0;
        this.updateInterval = 0.2;

        this.labelTipsTop = this.node.getChildByName("labelTipsTop");
        this.labelTipsTop.active = false;
        this.labelTipsBottom = this.node.getChildByName("labelTipsBottom");
        this.labelTipsBottom.active = false;

        this.titleTemplate = cc.find("view/content/title", this.node);
        this.titleTemplate.parent = null;
        this._rowHeight = this.titleTemplate.height;

        this.view = this.node.getChildByName("view");
        this.content = this.view.getChildByName("content");

        this.titlesPool = [];
        this.titlesList = [];
        this.dataList = null; 
        this.actPrePage = false;
        this.actNextPage = false;

        this.node.on("bounce-top", this.onBounceTop, this);
        this.node.on("bounce-bottom", this.onBounceBottom, this);

        this.init(null, null, null, 1, 2);
        let dataList = [];
        for (let k = 0; k < this._pageSize; k++) {
           dataList.push(k) ;
        }
        this.updateData(1, 2, dataList);
    },

    update(dt) {
        if (!this.isInit) {
           return; 
        }

        this.updateTimer += dt
        if (this.updateTimer < this.updateInterval) {
            return; 
        }
        this.updateTimer = 0;

        let bottomY = this.content.y - this.content.height;
        if (bottomY >= -this.view.height/2 + 30) {
            if (!this.labelTipsBottom.active) {
                if (this._pageIndex >= this._pageCount) {
                    this.labelTipsBottom.getComponent(cc.Label).string = "没有更多了";
                    this.labelTipsBottom.active = true;
                    this.actNextPage = false;

                } else {
                    this.labelTipsBottom.getComponent(cc.Label).string = "下一页";
                    this.labelTipsBottom.active = true;
                    this.actNextPage = true;
                }
            }

        } else {
            if (this.labelTipsBottom.active) {
                this.actNextPage = false;
                this.labelTipsBottom.active = false;
            }
            if (this.content.y <= this.view.height/2 - 30) {
                if (!this.labelTipsTop.active) {
                    if (this._pageIndex <= 1) {
                        this.labelTipsTop.getComponent(cc.Label).string = "没有更多了";
                        this.labelTipsTop.active = true;
                        this.actPrePage = false;
                    } else {
                        this.labelTipsTop.getComponent(cc.Label).string = "上一页";
                        this.labelTipsTop.active = true;
                        this.actPrePage = true;
                    }
                }
            } else {
                if (this.labelTipsTop.active) {
                    this.labelTipsTop.active = false;
                    this.actPrePage = false;
                }
            }
        }
    },

    //初始化
    //@rowSpacing 行距
    //@itemSpacing 列距
    //@pageSize 行/页
    //@pageIndex 页码
    //@pageCount 页数
    init(rowSpacing, itemSpacing, pageSize, pageIndex, pageCount) {
        this.isInit = true;
        this._rowSpacing = rowSpacing ? rowSpacing : this._rowSpacing;
        this._itemSpacing = itemSpacing ? itemSpacing : this._itemSpacing;
        this._pageSize = pageSize ? pageSize : this._pageSize;
        this._pageIndex = pageIndex ? pageIndex : this._pageIndex;
        this._pageCount = pageCount ? pageCount : this._pageCount;
    },

    rowSpacing() {
        return this._rowSpacing;
    },

    itemSpacing() {
        return this._itemSpacing;
    },

    pageSize() {
        return this._pageSize;
    },

    pageIndex() {
        return this._pageIndex;
    },

    pageCount() {
        return this._pageCount;
    },

    //更新数据
    updateData(pageIndex, pageCount, dataList) {
        this.clearList();
        this._pageIndex = pageIndex;
        this._pageCount = pageCount;
        this.dataList = dataList;

        let contentHeight = 0;
        for (let k=0; k<dataList.length; k++) {
            let title = this.createTitle(dataList[k]);
            if (title) {
                title.x = 0;
                title.y = -k*(title.height+this._rowSpacing);
                title.parent = this.content;
                this.titlesList.push(title);
                contentHeight += title.height + this._rowSpacing;
            }
        }
        this.content.height = contentHeight;
        if (this.content.height < this.view.height) {
            this.content.height = this.view.height + this.titleTemplate.height;
        }
    },

    //下一页(子类覆盖)
    nextPage() {
        let dataList = [];
        for (let k = 0; k < this._pageSize; k++) {
           dataList.push(k);
        }
        this.updateData(this._pageIndex+1, 2, dataList);
    },

    //上一页(子类覆盖)
    prePage() {
        let dataList = [];
        for (let k = 0; k < this._pageSize; k++) {
           dataList.push(k) ;
        }
        this.updateData(this._pageIndex-1, 2, dataList);
    },

    //创建title默认函数(子类覆盖)
    createTitle(data) {
        let title = this.getPoolTitle();   
        if (!title) {
            title = cc.instantiate(this.titleTemplate);
        }
        return title;
    },

    //从title池子中取1个title
    getPoolTitle() {
        if (this.titlesPool.length <= 0) {
            return; 
        }
        let title = this.titlesPool.pop();
        return title;
    },
    
    //清空列表,并把title放到title池子
    clearList() {
        let game = require("game");
        for (let k = 0; k < this.titlesList.length; k++) {
            let title = this.titlesList[k];
            title.parent = null;
            this.titlesPool.push(title);
        }
        this.titlesList = [];
        this.content.height = this.view.height + this.titleTemplate.height;
    },

    onBounceTop(scrollView) {
        if (this.actPrePage) {
            this.prePage(); 
        }
    },

    onBounceBottom(scrollView) {
        if (this.actNextPage) {
            this.nextPage(); 
        }
    },
});
