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
let regCmd = require("regCmd");
let packet = require("packet");
let cltPBRpc = require("cltPBRpc");
let cltCmdRpc = require("cltCmdRpc");
let protoBuilder = require("protoBuilder");

cc.Class({
    extends: cc.Component,

    properties: {
		//变量
		socket: null,
		lastURL: null,
		successTimes: 0, 	//连接成功的次数
		//包编号
		packetIndex: 0,
		//回调函数
		fnOpenCallback: null,
		fnCloseCallback: null,
		fnReconCallback: null,
		callbackParam: null,
		//心跳相关
		hbCloseTime: 30, 	//判定断开时间(秒)
		hbIntvalTime: 10,	//心跳间隔时间(秒)
		hbLastRecvTime: 0, 	//上1次收到心跳包时间
		//重连相关
		reconnecting: false,//是否正在重连
		reconTimes: 0,		//已重连次数
		maxReconTimes: 6,	//最大重连次数
		reconIntvalTime: 6,	//重连间隔时间(秒)
		lastReconTime: 0,	//上一重连时间
		
		packetCache: [], 	//缓存网络包,按帧处理
    },

    ctor() {
    	this.schedule(this.update, 1);
    },

	//不是关闭的不等于是开着的
	isClosed() {
		if (!this.socket) {
			return true;
		}
		return this.socket.readyState == WebSocket.CLOSED;
	},

	//不是开着的不等于是关闭的
	isOpen() {
		if (!this.socket) {
			return false;
		}
		return this.socket.readyState == WebSocket.OPEN;
	},

	//关闭连接
	close(notReconnect=false) {
		if (notReconnect) { //不重连
		    this.successTimes = 0;
		}

		if (this.socket) {
			this.socket.close();
			this.socket = null
		}
	},

	//定时器(每秒更新)
	update() {
		//心跳
		if (this.isOpen()) {
			let time = misc.timeStamp();
			if (time % this.hbIntvalTime === 0) {
				this.heartBeatReq(time);
			}
			if (this.hbLastRecvTime == 0) {
				this.hbLastRecvTime = time;
			}
			if (time - this.hbLastRecvTime >= this.hbCloseTime) {
				console.log("心跳超时连接死亡");
				this.close();
			}
		} else {
			if (this.reconnecting && misc.timeStamp() >= this.lastReconTime + this.reconIntvalTime) {
				this.doReconnect();
			}
		}
	},

	heartBeatReq(time) {
		packet.new();
		packet.pushInt32(time);
		this.cmdCall("HeartBeatReq", packet);
	},

	heartBeatRet(time) {
		this.hbLastRecvTime = misc.timeStamp();
	},

	//发起连接
	connect(ws_url=null, _fnOpenCallback=null, _fnCloseCallback=null, _fnReconCallback=null, _param=null) {
	    misc.assert(ws_url != "", "地址非法");
	    console.log("服务器地址:", ws_url);
		this.lastURL = ws_url;

		this.close();
		this.socket = new WebSocket(this.lastURL);
		this.socket.onopen = this.onOpen;
		this.socket.onmessage = this.onMessage;
		this.socket.onerror = this.onError;
		this.socket.onclose = this.onClose;
		this.fnOpenCallback = _fnOpenCallback;
		this.fnCloseCallback = _fnCloseCallback;
		this.fnReconCallback = _fnReconCallback;
		this.callbackParam = _param;
	},

	onError(event) {
		console.error("网络错误.", event);
	},

	onOpen(event) {
		this.socket.binaryType = 'arraybuffer';
		this.successTimes++;

		//是否重连判断
		let isReconnect = false;
		if (this.reconnecting) {
			isReconnect = true;
			this.stopReconnect();
		}

		//连接成功回调
		if (this.fnOpenCallback) {
			this.fnOpenCallback.call(this.callbackParam, isReconnect);
		}

		//每帧处理
		this.schedule(this.packetTimer, 1000/60);
	},

	onClose(event) {
		console.error("网络已关闭.", event);
		this.packetIndex = 0;
		this.packetCache = [];
		this.hbLastRecvTime = 0;
		this.unschedule(this.packetTimer);

		//连接断开回调
		if (this.fnCloseCallback) {
			this.fnCloseCallback.call(this.callbackParam);
		}

		//进行重连检测
		if (this.successTimes > 0) {
			this.startReconnect();
		}
	},

	//开始重连流程
	startReconnect() {
		if (this.reconnecting) {
			return;
		}
		this.reconnecting = true;
		this.lastReconTime = 0;
	},

	//重连操作
	doReconnect() {
		this.reconTimes++;
		if (this.reconTimes > this.maxReconTimes) {
			this.stopReconnect();
			console.error("重连失败***");
			if (this.fnReconCallback) {
				this.fnReconCallback.call(this.callbackParam, 0)
			}
		} else {
			console.log("重连事件***", this.reconTimes);
			if (this.fnReconCallback) {
				this.fnReconCallback.call(this.callbackParam, this.reconTimes);
			}
			this.connect(this.fnOpenCallback, this.fnCloseCallback, this.fnReconCallback, this.callbackParam);
			this.lastReconTime = misc.timeStamp();
		}
	},

	//停止重连流程
	stopReconnect() {
		this.reconTimes = 0;
		this.lastReconTime = 0;
		this.reconnecting = false;
	},

	//生成包编号
	genPacketIndex() {
		this.packetIndex = this.packetIndex % 0x7FFFFFFF + 1;
		return this.packetIndex;
	},

	//收到包
	onMessage(event) {
		let buffer = event.data;
		let len = buffer.byteLength;

		let view = new DataView(buffer);
		let realLen = view.getInt32(0, true);
		let cmdNo = view.getUint16(len-8, true);
		let source = view.getInt8(len-6, true);
		let target = view.getInt8(len-5, true);
		let index  = view.getUint32(len-4, true);

		if (cmdNo >= regCmd.cmdRange[0] && cmdNo <= regCmd.cmdRange[1]) {
			let cmd = regCmd.getCmdRet(cmdNo);
			misc.assert(cmd, "找不到下行协议:"+cmdNo);

			let data = buffer.slice(4, len-8);
			data = packet.parse(data, cmd[2]);

			if (cmdNo == 1100) {
				this.heartBeatRet(data[0]);

			} else if (cltCmdRpc.dispatchCmdMsg) {
				this.packetCache.push([1, cmdNo, cmd[1], source, cmd[4], data])

			} else {
				misc.assert(false, "找不到处理函数:"+cmd[1]);

			}

		} else if (cmdNo >= regCmd.pbCmdRange[0] && cmdNo <= regCmd.pbCmdRange[1]) {
			let cmd = regCmd.getPBRet(cmdNo);
			misc.assert(cmd, "找不到下行协议:"+cmdNo);

			let data = buffer.slice(4, len-8);
			data = protoBuilder.decode(data, cmd[2]);

			if (cltPBRpc.dispatchPBMsg) {
				this.packetCache.push([2, cmdNo, cmd[1], source, cmd[4], data])

			} else {
				misc.assert(false, "找不到处理函数:"+cmd[1]);

			}

		} else {
			misc.assert(false, "非法指令号:"+cmdNo);

		}
	},

	packetTimer() {
		if (this.packetCache.length > 0) {
			let cmd = this.packetCache.shift();
			if (cmd[0] == 1) {
				cltCmdRpc.dispatchCmdMsg(cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);

			} else if (cmd[0] == 2) {
				cltPBRpc.dispatchPBMsg(cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
			}
		}
	},

	//发送自定义包
	cmdCall(cmdName, packet) {
		if (!this.isOpen()) {
			console.error("网络未打开或已关闭");
			return;
		}
		let cmd = regCmd.getCmdReq(cmdName);
		misc.assert(cmd, "找不到上行协议:"+cmdName);

		//添加头部
		let cmdNo = cmd[0]	
		let target = cmd[3]
		let source = 0;
		let newIndex = this.genPacketIndex();

		packet.pushUint16(cmdNo);
		packet.pushInt8(source);
		packet.pushInt8(target);
		packet.pushUint32(newIndex);

	    //发送
		this.socket.send(packet.buffer().slice(0, packet.size()));
	},

	//发送PROTOBUF包
	pbCall(cmdName, packet) {
		if (!this.isOpen()) {
			console.error("网络未打开或已关闭.");
			return;
		}
		let cmd = regCmd.getPBReq(cmdName);
		misc.assert(cmd, "找不到上行协议:"+cmdName);

		//添加头部
		let cmdNo = cmd[0];
		let target = cmd[3];
		let source = 0;
		let newIndex = this.genPacketIndex();

	    let byteBuffer = packet.encode();
	    byteBuffer.offset = byteBuffer.toBuffer().byteLength;
		byteBuffer.writeUint16(cmdNo);
		byteBuffer.writeInt8(source);
		byteBuffer.writeInt8(target);
		byteBuffer.writeUint32(newIndex);
	    byteBuffer.flip();
		byteBuffer.writeInt32(byteBuffer.toBuffer().byteLength-4, 0);

	    //发送
		socket.send(byteBuffer.toBuffer());
	},

});
