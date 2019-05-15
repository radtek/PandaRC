#pragma once
#include <string.h>
#include <stdint.h>
#include <WinSock2.h>

namespace NSWSD
{
	//websocket 头部
	struct WSHeader
	{
		uint8_t fin_;
		uint8_t opcode_;
		uint8_t mask_;
		uint8_t masking_key_[4];
		uint64_t payload_length_;
		void Reset()
		{
			fin_ = 0;
			opcode_ = 0;
			mask_ = 0;
			payload_length_ = 0;
			memset(masking_key_, 0, sizeof(masking_key_));
		}
	};
	WSHeader oWSHeader;

	void EndianSwap(uint8_t *pData, int startIndex, int length)
	{
		int i, cnt, end, start;
		cnt = length / 2;
		start = startIndex;
		end = startIndex + length - 1;
		uint8_t tmp;
		for (i = 0; i < cnt; i++)
		{
			tmp = pData[start + i];
			pData[start + i] = pData[end - i];
			pData[end - i] = tmp;
		}
	}

	void WSMaskDecode(uint8_t* pData, int nLen)
	{
		if (oWSHeader.mask_ == 1)
		{
			for (int i = 0; i < nLen; i++)
			{
				int j = i % 4;
				pData[i] = pData[i] ^ oWSHeader.masking_key_[j];
			}
		}
	}

	unsigned long long N2Hll(unsigned long long val)
	{
#ifdef __linux
		if (__BYTE_ORDER == __LITTLE_ENDIAN)
		{
			return (((unsigned long long)htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
		}
		else if (__BYTE_ORDER == __BIG_ENDIAN)
		{
			return val;
		}
#else
		return ntohll(val);
#endif
	}

	int DecodePacket(uint8_t* pData, int nLen, int nHeaderSize, uint8_t* pOutData, int& nOutLen)
	{
		//fin_
		if (nLen <= 0)
		{
			return -1;
		}
		uint8_t* pPos = pData;
		uint8_t* pPosEnd = pData + nLen;

		oWSHeader.Reset();
		oWSHeader.fin_ = (*pPos) >> 7;
		//opcode_
		oWSHeader.opcode_ = (*pPos) & 0x0f;
		pPos++;

		//mask_
		if (pPos >= pPosEnd)
		{
			return -2;
		}
		oWSHeader.mask_ = (*pPos) >> 7;
		//payload_length_
		oWSHeader.payload_length_ = (*pPos) & 0x7f;
		pPos++;

		//extended payload_length_
		if (oWSHeader.payload_length_ == 126)
		{
			if (pPos + 2 > pPosEnd)
			{
				return -3;
			}
			uint16_t uLength = 0;
			memcpy(&uLength, pPos, 2);
			pPos += 2;
			oWSHeader.payload_length_ = ntohs(uLength);
		}
		else if (oWSHeader.payload_length_ == 127)
		{
			if (pPos + 8 > pPosEnd)
			{
				return -4;
			}
			uint64_t uLength = 0;
			memcpy(&uLength, pPos, 8);
			pPos += 8;
			oWSHeader.payload_length_ = N2Hll(uLength);
		}

		//连接关闭
		if (oWSHeader.opcode_ == 0x8)
		{
			return -5;
		}

		//mask_key_
		if (oWSHeader.mask_ == 1)
		{
			if (pPos + 4 > pPosEnd)
			{
				return -6;
			}
			for (int i = 0; i < 4; i++)
			{
				oWSHeader.masking_key_[i] = *(pPos + i);
			}
			pPos += 4;
		}
		//printf("websocket header:%d payload:%d\n", pPos-pData, oWSHeader.payload_length_);


		//////切真正的数据包//////
		uint8_t*pTmpPos = pPos;
		int nSize = (int)(pPosEnd - pPos);
		if (nSize < nHeaderSize)
		{
			return -7;
		}
		WSMaskDecode(pTmpPos, nSize);
		memcpy(pOutData, pTmpPos, nSize);
		nOutLen = nSize;
		return 0;
	}

	int EncodePacket(uint8_t* pData, int nLen, uint8_t* pOutData, int& nOutLen)
	{
		uint8_t sHeader[14];
		uint8_t* pPos = sHeader;
		int nHeaderLen = 2;


		pPos[0] = oWSHeader.fin_ << 7 | oWSHeader.opcode_;
		pPos[1] = oWSHeader.mask_ << 7 | oWSHeader.payload_length_;
		if (nLen < 126)
		{
			pPos[1] |= nLen;
		}
		else if (nLen < 65536)
		{
			pPos[1] |= 126;
			pPos[2] = nLen >> 8;
			pPos[3] = nLen & 0xFF;
			nHeaderLen += 2;
		}
		else
		{
			pPos[1] |= 127;
			//只支4字节整型大小的包
			pPos[2] = 0;
			pPos[3] = 0;
			pPos[4] = 0;
			pPos[5] = 0;
			pPos[6] = nLen >> 24;
			pPos[7] = nLen >> 16;
			pPos[8] = nLen >> 8;
			pPos[9] = nLen & 0xFF;
			memcpy((void*)(pPos + 2), &nLen, 8);
			nHeaderLen += 8;
		}
		if (oWSHeader.mask_ == 1)
		{
			uint8_t* pTmp = pPos + nHeaderLen;
			for (int i = 0; i < 4; i++)
			{
				pTmp[i] = oWSHeader.masking_key_[i];
			}
			nHeaderLen += 4;
		}
		//printf("header size:%d\n", nHeaderLen);
		memcpy(pOutData, sHeader, nHeaderLen);
		memcpy(pOutData + nHeaderLen, pData, nLen);
		WSMaskDecode(pOutData + nHeaderLen, nLen);
		nOutLen = nHeaderLen + nLen;
		return 0;
	}
}
