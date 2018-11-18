#pragma once
#include <string>
#include <QString>
#include <QNetworkInterface>

namespace NSMisc
{

	std::string getHostMac()
	{
		QString strMacAddr = "";
		QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces(); // 获取所有网络接口列表
		for (int i = 0; i < nets.count(); i++)
		{
			// 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
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
