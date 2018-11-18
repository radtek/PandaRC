#pragma once
#include <string>
#include <QString>
#include <QNetworkInterface>

namespace NSMisc
{

	std::string getHostMac()
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
};
