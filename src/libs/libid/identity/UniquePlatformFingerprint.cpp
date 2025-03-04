#include "identity/UniquePlatformFingerprint.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QNetworkInterface>
#include <QList>
#include <QHostAddress>
#include <QNetworkInformation>
#include <QFileInfo>
#include <QCoreApplication>




UniquePlatformFingerprint *UniquePlatformFingerprint::instance=nullptr;


UniquePlatformFingerprint::UniquePlatformFingerprint()
{
	OC_METHODGATE();
	//	QNetworkInterface *inter=OC_NEW QNetworkInterface();
	QList<QNetworkInterface> interfaceList=QNetworkInterface::allInterfaces();
	for(QList<QNetworkInterface>::iterator it=interfaceList.begin(),eit=interfaceList.end(); it!=eit; ++it) {
		QNetworkInterface iface=(*it);
		iface.hardwareAddress();
		QNetworkInterface::InterfaceFlags flags=iface.flags();
		if(flags&QNetworkInterface::InterfaceFlag::IsLoopBack) {
			continue;
		}
		if(flags&QNetworkInterface::InterfaceFlag::IsPointToPoint) {
			continue;
		}
		const QString mac=iface.hardwareAddress().toUpper().replace("[^0-9A-F]","").trimmed();

		if(""!=mac) {
			platformPrint=Fingerprint(mac);
			break;
		}
	}
	QString fn=QFileInfo( QCoreApplication::applicationFilePath()).fileName();
	executablePrint=Fingerprint(fn);
//	qDebug()<<"--- E X E C U T A B L E : "<<fn<<" => "<<executablePrint.toString();
}



//TODO: REPLACE SINGLETON
UniquePlatformFingerprint & UniquePlatformFingerprint::getInstance()
{
	OC_FUNCTIONGATE();
	if(nullptr==instance) {
		instance=OC_NEW UniquePlatformFingerprint();
	}
	return *instance;
}




const Fingerprint &UniquePlatformFingerprint::platform() const
{
	OC_METHODGATE();
	return platformPrint;
}

const  Fingerprint &UniquePlatformFingerprint::executable() const
{
	OC_METHODGATE();
	return executablePrint;
}

