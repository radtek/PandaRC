#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<iostream>
using namespace std;

#include "websocket.h"


int main()
{
	int t[] = {0x81,0xB7,0xA4,0x13,0xAD,0x46,0xDF,0x31,0xC0,0x35,0xC3,0x5A,0xC9,0x64,0x9E,0x22,0x9C,0x6A,0x86,0x67,0xC4,0x2B,0xC1,0x31,0x97,0x77,0x91,0x26,0x9F,0x7E,0x95,0x22,0x94,0x77,0x94,0x20,0x9A,0x7E,0x88,0x31,0xC9,0x27,0xD0,0x72,0x8F,0x7C,0xDF,0x31,0xCE,0x2B,0xC0,0x31,0x97,0x64,0xD4,0x7A,0xC3,0x21,0x86,0x6E,0xD0};

	const int nPacketSize = sizeof(t)/sizeof(int);
	uint8_t d[nPacketSize] = {};
	for (int i = 0; i < nPacketSize; i++) {
		d[i] = (int8_t)(t[i]);
		printf("%X ", t[i]);
	}
	printf("\n");

	int sizeBytes = 0; //长度占字节数

	int decodeSize = 0;
	uint8_t decodeData[1024] = {0};
	cout<<"packet size:"<<nPacketSize<<" decode res:"<<DecodePacket(d, sizeof(d), sizeBytes, decodeData, decodeSize)<<endl;
	cout<<"data size: "<<decodeSize<<endl;
	printf("try print: %s\n\n", decodeData);

	cout<<"please input target data:"<<endl;
	uint8_t targetData[1024] = {0};
	fgets((char*)targetData, sizeof(targetData)-1, stdin);//会读取换行符
	cout<<"input: "<<(char*)targetData;

	int encodeSize = 0;
	uint8_t encodeData[1024] = {0};
	EncodePacket(targetData, strlen((char*)targetData)-1, encodeData, encodeSize);
	cout<<"packet size:"<<encodeSize<<endl;
	for (int i = 0; i < encodeSize; i++) {
		printf("%X ", encodeData[i]);
	}

	return 0;
}
