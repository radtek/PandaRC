//注册网络处理函数模块
var regCmd = require("regCmd");

//////注册自定义协议
function RegCmdReq(cmdNo, cmdName, proto, target) {
	regCmd.regCmdReq(cmdNo, cmdName, proto, target);
};

function RegCmdRet(cmdNo, cmdName, proto, target) {
	regCmd.regCmdRet(cmdNo, cmdName, proto, target);
};

//心跳
RegCmdReq(1100, "HeartBeatReq", "i", 0);
RegCmdRet(1100, "HeartBeatRet", "i", 0);




//////注册PROTOBUF协议,指令范围:[8001, 40000]
function RegPBReq(cmdNo, cmdName, proto, target) {
	regCmd.regPBReq(cmdNo, cmdName, proto, target);
};

function RegPBRet(cmdNo, cmdName, proto, target) {
	regCmd.regPBRet(cmdNo, cmdName, proto, target);
};

//GM
RegPBReq(8001, "GMCmdReq", "global.GMCmdReq", 0)

//TEST
RegPBReq(8004, "TestPack", "global.TestPack", 0)
RegPBRet(8004, "TestPack", "global.TestPack", 0)

// //LOGIN
// RegPBReq(9001, "LoginReq", "login.LoginReq", 0)							//登录请求
// RegPBRet(9002, "LoginRet", "login.LoginRet", 0)							//登录返回
// RegPBReq(9003, "AccountIDReq", "login.AccountIDReq", 0)					//账号ID请求
// RegPBRet(9004, "AccountIDRet", "login.AccountIDRet", 0)					//账号ID返回

// //九死一生
// RegPBRet(9101, "PlayerEnterRet", "ninedead.PlayerEnterRet", 0)			//玩家进入房间通知(广播)
// RegPBReq(9102, "PlayerReadyReq", "ninedead.PlayerReadyReq", 0)			//玩家准备请求
// RegPBReq(9103, "PlayerLeaveReq", "ninedead.PlayerLeaveReq", 0)			//玩家离开请求(会结束游戏)
// RegPBRet(9104, "PlayerLeaveRet", "ninedead.PlayerLeaveRet", 0)			//玩家离开返回(广播)
// RegPBReq(9105, "PlayerMoveReq", "ninedead.PlayerMoveReq", 0)			//玩家移动请求
// RegPBRet(9106, "PlayerMoveRet", "ninedead.PlayerMoveRet", 0)			//玩家移动返回(广播)
// RegPBReq(9107, "PlayerDeadReq", "ninedead.PlayerDeadReq", 0)			//玩家死亡请求
// RegPBRet(9108, "PlayerDeadRet", "ninedead.PlayerDeadRet", 0)			//玩家死亡返回(广播)
// RegPBRet(9109, "StateChangeRet", "ninedead.StateChangeRet", 0)			//游戏状态改变(广播)
// RegPBRet(9110, "BombAppearRet", "ninedead.BombAppearRet", 0)			//炸弹出现返回(广播)
// RegPBRet(9111, "GameFinishRet", "ninedead.GameFinishRet", 0)			//游戏结束返回(广播)
