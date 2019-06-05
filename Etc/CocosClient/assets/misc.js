// Learn cc.Class:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/class.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/class.html
// Learn Attribute:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] http://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/en/scripting/life-cycle-callbacks.html

let comMisc = cc.Class({
    extends: cc.Component,

    properties: {
    },

    ctor() {

    },

    //断言
    assert(res, text="") {
        if (!(res)) {
            throw new Error("assert fail: " + text);
        }
    },

    //时间戳(整秒)
    timeStamp() {
        var timeStamp = new Date().getTime();
        return Math.floor(timeStamp / 1000);
    },

    //时间戳(毫秒)
    msTime() {
        var timeStamp = Math.floor(new Date().getTime());
        return timeStamp;
    },

    //html参数
    getQueryString(name) {
        var reg = new RegExp("(^|&)"+ name +"=([^&]*)(&|$)");
        var r = window.location.search.substr(1).match(reg);
        if(r!=null) return  unescape(r[2]); return null;
    },

     //获取字符串里面的参数
	getQuerySubString(substr, name) {
        var reg = new RegExp("(^|&)"+ name +"=([^&]*)(&|$)");
        var r = substr.match(reg);
        if(r!=null) return unescape(r[2]); return null;
    },

    //加载客户端配置(打包index.html所在目录的json文件)
    loadConf(name, fnCallback) {
        let url = window.location.href;
        cc.loader.load(url+"/"+name, function(err, data) {
            if (err) {
                console.error(err);
                fnCallback(null);
                return;
            }
            fnCallback(data);
        })
    },
});




module.exports = new comMisc();
