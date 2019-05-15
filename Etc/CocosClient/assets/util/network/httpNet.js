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

    //同步GET
    //@url example: http://www.abc.com?a=1&b=2
    httpGet(url) {
        var request = new XMLHttpRequest();
        request.open("GET", url, false);
        request.send();
        return request.responseText;
    },

    //同步POST
    //@url example: http://www.abc.com
    //@data example: a=1&b=2
    httpPost(url, data="") {
        var request = new XMLHttpRequest();
        request.open("POST", url, false);
        //For cross-domain requests, setting the content type to anything other than application/x-www-form-urlencoded,
        //multipart/form-data, or text/plain will trigger the browser to send a preflight OPTIONS request to the server.
        request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
        request.send(data);
        return request.responseText;
    },

    //异步GET
    //@url example: http://www.abc.com?a=1&b=2
    asyncGet(url, callback=null) {
        var request = new XMLHttpRequest();
        request.onreadystatechange = function () {
            if (request.readyState == 4 && (request.status >= 200 && request.status < 400)) {
                var response = request.responseText;
                if (callback) {
                    callback(response);
                } else {
                    console.log(response);
                }
            }
        };
        request.responseType = "text";
        request.open("GET", url, true);
        request.send();
    },

    //异步POST
    //@url example: http://www.abc.com
    //@data example: a=1&b=2
    asyncPost(url, data="", callback=null) {
        var request = new XMLHttpRequest();
        request.onreadystatechange = function () {
            if (request.readyState == 4 && (request.status >= 200 && request.status < 400)) {
                var response = request.responseText;
                if (callback) {
                    callback(response);
                } else {
                    console.log("asyncpost ret:", response);
                }
            }
        };
        request.responseType = "text";
        request.open("POST", url, true);
        request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
        request.send(data);
    },

});
