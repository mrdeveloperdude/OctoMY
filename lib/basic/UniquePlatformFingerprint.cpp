#include "basic/UniquePlatformFingerprint.hpp"
#include <QNetworkInterface>
#include <QList>
#include <QHostAddress>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QCryptographicHash>


#include "utility/BaseTranscode.hpp"

UniquePlatformFingerprint::UniquePlatformFingerprint():id(""),b32ww(""){
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
			QCryptographicHash hash(QCryptographicHash::Sha1);
			hash.addData("30691772764913072071865163056420114973107080075087047");
			QByteArray ba=mac.toLatin1();
			hash.addData(ba.data());
			raw=hash.result();
			id=raw.toHex().toUpper();
			b32ww=BaseTranscode::transcode(BaseTranscode::Bytes, BaseTranscode::Base32ww,raw);
			break;
		}

	}

}

UniquePlatformFingerprint *UniquePlatformFingerprint::instance=0;

//TODO: REPLACE SINGLETON
UniquePlatformFingerprint & UniquePlatformFingerprint::getInstance(){
	if(0==instance){
		instance=new UniquePlatformFingerprint();
	}
	return *instance;
}

#include "basic/Settings.hpp"
QString UniquePlatformFingerprint::getHEX(){
	const QString IMPOSSIBLE="IMPOSSIBLE";
	const QString override=Settings::getInstance().getCustomSetting("fingerprint-override",IMPOSSIBLE);
	if(IMPOSSIBLE==override){
		return id;
	}
	else{
		return override;
	}
}

QString UniquePlatformFingerprint::getBase32ww(){
	return b32ww;
}


QByteArray UniquePlatformFingerprint::getRaw(){
	return raw;
}

bool UniquePlatformFingerprint::isValid(){
	return ""!=id;
}


