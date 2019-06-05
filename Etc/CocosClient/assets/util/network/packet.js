//自定义包
let BUFF_SIZE = 2048;
let SMALL_ENDIAN = true; //小端
let arrayBuf = new ArrayBuffer(BUFF_SIZE);
let dataView = new DataView(arrayBuf);
let readedSize = 0;	//已读取字节数
let offsetByte = 4; //偏移量
let writedSize = offsetByte; //已写入字节数

let packet = {};

packet.new = function () {
	readedSize = 0;
	writedSize = offsetByte;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.buffer = function () {
	return dataView.buffer;
};

packet.size = function() {
	return writedSize;
};

//抛出错误
function raseError() {
	throw new Error("packet.js: 数据超出缓冲区大小:"+BUFF_SIZE);	
};

packet.pushInt8 = function (value) {
	if (writedSize >= BUFF_SIZE) {
		return raseError();
	}
	dataView.setInt8(writedSize, value, SMALL_ENDIAN);
	writedSize++;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushUint8 = function (value) {
	if (writedSize >= BUFF_SIZE) {
		return raseError();
	}
	dataView.setUint8(writedSize, value, SMALL_ENDIAN);
	writedSize++;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushInt16 = function (value) {
	if (writedSize + 2 > BUFF_SIZE) {
		return raseError();
	}
	dataView.setInt16(writedSize, value, SMALL_ENDIAN);
	writedSize += 2;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushUint16 = function (value) {
	if (writedSize + 2 > BUFF_SIZE) {
		return raseError();
	}
	dataView.setUint16(writedSize, value, SMALL_ENDIAN);
	writedSize += 2;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushInt32 = function (value) {
	if (writedSize + 4 > BUFF_SIZE) {
		return raseError();
	}
	dataView.setInt32(writedSize, value, SMALL_ENDIAN);
	writedSize += 4;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushUint32 = function (value) {
	if (writedSize + 4 > BUFF_SIZE) {
		return raseError();
	}
	dataView.setUint32(writedSize, value, SMALL_ENDIAN);
	writedSize += 4;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushFloat32 = function (value) {
	if (writedSize + 4 > BUFF_SIZE) {
		return raseError();
	}
	dataView.setFloat32(writedSize, value, SMALL_ENDIAN);
	writedSize += 4;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushFloat64 = function (value) {
	if (writedSize + 8 > BUFF_SIZE) {
		return raseError();
	}
	dataView.setFloat64(writedSize, value, SMALL_ENDIAN);
	writedSize += 8;
	dataView.setInt32(0, writedSize-offsetByte, SMALL_ENDIAN);
};

packet.pushStr = function (value) {
	strLen = value.length;
	if (writedSize + strLen > BUFF_SIZE) {
		return raseError();
	}
	this.pushInt32(strLen);
	for (var i = 0; i < strLen; i++) {
		this.pushUint16(value.charCodeAt(i));
	}
};

packet.size = function () {
	return writedSize;
};	

packet.parse = function (buffer, proto) {
	var data = [];
	var view = new DataView(buffer);
	var offset = 0;
	for (var i = 0; i < proto.length; i++) {
		if (proto.charAt(i) == "i") {
			data[i] = view.getInt32(offset);
			offset += 4;
		}
	}
	return data;
};

module.exports = packet;