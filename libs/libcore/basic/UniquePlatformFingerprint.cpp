#include "basic/UniquePlatformFingerprint.hpp"
#include <QNetworkInterface>
#include <QList>
#include <QHostAddress>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QFileInfo>
#include <QCoreApplication>




////////////////////////////////////////////////////////////////////////////////

UniquePlatformFingerprint::UniquePlatformFingerprint(){
	//	QNetworkInterface *inter=new QNetworkInterface();
	QList<QNetworkInterface> interfaceList=QNetworkInterface::allInterfaces();
	for(QList<QNetworkInterface>::iterator it=interfaceList.begin(),eit=interfaceList.end();it!=eit;++it){
		QNetworkInterface iface=(*it);
		iface.hardwareAddress();
		QNetworkInterface::InterfaceFlags flags=iface.flags();
		if(flags&QNetworkInterface::InterfaceFlag::IsLoopBack){
			continue;
		}
		if(flags&QNetworkInterface::InterfaceFlag::IsPointToPoint){
			continue;
		}
		const QString mac=iface.hardwareAddress().toUpper().replace("[^0-9A-F]","").trimmed();

		if(""!=mac){
			platformPrint=Fingerprint(mac);
			break;
		}
	}
	QString fn=QFileInfo( QCoreApplication::applicationFilePath()).fileName();
	executablePrint=Fingerprint(fn);
//	qDebug()<<"--- E X E C U T A B L E : "<<fn<<" => "<<executablePrint.toString();
}

UniquePlatformFingerprint *UniquePlatformFingerprint::instance=0;

//TODO: REPLACE SINGLETON
UniquePlatformFingerprint & UniquePlatformFingerprint::getInstance(){
	if(0==instance){
		instance=new UniquePlatformFingerprint();
	}
	return *instance;
}




const Fingerprint &UniquePlatformFingerprint::platform() const {
	return platformPrint;
}

const  Fingerprint &UniquePlatformFingerprint::executable() const {
	return executablePrint;
}
