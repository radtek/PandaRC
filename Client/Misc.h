#pragma once
#include <string>
#include <QString>
#include <QNetworkInterface>
#include "zlib/zlib.h"

namespace NSMisc
{

	static std::string getHostMac()
	{
		QString strMacAddr = "";
		QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces(); // ��ȡ��������ӿ��б�
		for (int i = 0; i < nets.count(); i++)
		{
			// ���������ӿڱ�������������в��Ҳ��ǻػ���ַ�������������Ҫ�ҵ�Mac��ַ
			if (nets[i].flags().testFlag(QNetworkInterface::IsUp)
				&& nets[i].flags().testFlag(QNetworkInterface::IsRunning)
				&& !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
			{
				strMacAddr = nets[i].hardwareAddress();
				break;
			}
		}
		return strMacAddr.toStdString();
	}

	static bool zipCompress(uint8_t* inbuffer, int insize, uint8_t** outbuffer, int& outsize)
	{
		static uint8_t* buffer = NULL;
		static int size = 0;
		uint32_t bound = compressBound(insize);
		if (size < bound)
		{
			buffer = (uint8_t*)realloc(buffer, bound);
			size = bound;
		}
		uint32_t destSize = size;
		if (compress((Bytef *)buffer, (uLongf*)&destSize, (Bytef *)inbuffer, insize) != Z_OK)
		{
			*outbuffer = NULL;
			outsize = 0;
			return false;
		}
		*outbuffer = buffer;
		outsize = destSize;
		return true;
	}

	static bool zipUncompress(uint8_t* inbuffer, int insize, uint8_t** outbuffer, int& outsize)
	{
		static uint8_t* buffer = NULL;
		static int size = 10*1024*1024;

		if (buffer == NULL)
		{
			buffer = (uint8_t*)realloc(buffer, size);
		}
		uint32_t destSize = size;
		if (uncompress((Bytef *)buffer, (uLongf*)&destSize, (Bytef *)inbuffer, insize) != Z_OK)
		{
			*outbuffer = NULL;
			outsize = 0;
			return false;
		}
		*outbuffer = buffer;
		outsize = destSize;
		return true;
	}
};
