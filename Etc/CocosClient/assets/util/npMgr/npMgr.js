//节点对象池管理器,频繁创建和移除的节点用此管理器缓存,比如子弹,鱼,怪物等

let misc = require("misc");

cc.Class({
    extends: cc.Component,

    properties: {
        recordList: [],
        _npMap: { default: {}, },
    },

    ctor() {
        //这里定义一堆对象池,根据用途起名
        this._npMap["bullet"] = new cc.NodePool();
        this._npMap["commonTips"] = new cc.NodePool();
    },

    //@poolName 对象池名称,为了便于排错,不能自动创建
    getPool(poolName) {
        let pool = this._npMap[poolName];
        misc.assert(pool, "对象池不存在:"+poolName);
        return pool;
    },

    clearPool(poolName) {
        this.getPool(poolName).clear();
    },

    //@prefab 节点预制件,如果传了进来,池子里没有对象自动创建
    get(poolName, prefab) {
        let pool = this.getPool(poolName);
        let node = null;
        if (pool.size() > 0) {
            node = pool.get();
        } else if (prefab) {
            node = cc.instantiate(prefab);
        }

        //防止程序漏了回收导致泄漏
        // cc.loader.setAutoReleaseRecursively(node, true);
        return node;
    },

    //回收节点
    put(poolName, node) {
        //设置不自动回收
        // cc.loader.setAutoReleaseRecursively(node, false);

        let pool = this.getPool(poolName);
        pool.put(node);
        return pool.size();
    },

    size(poolName) {
        let pool = this.getPool(poolName);
        return pool.size();
    },
});
