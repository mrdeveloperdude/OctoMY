#include "basic/UniquePlatformFingerprint.hpp"
#include <QNetworkInterface>
#include <QList>
#include <QHostAddress>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QCoreApplication>


#include "utility/BaseTranscode.hpp"


Fingerprint::Fingerprint(const QString in)
	: in(in)
	, id("")
	, b32ww("")
	, qint(0)
	, quint(0)
{
	if(""!=in){
		QCryptographicHash hash(QCryptographicHash::Sha1);
		hash.addData("Robots running OctoMY™");
		raw=in.toUtf8();
		hash.addData(raw);
		hash.addData("will pave the way to true autonomy.");
		raw=hash.result();
		id=raw.toHex().toUpper();
		b32ww=BaseTranscode::transcode(BaseTranscode::Bytes, BaseTranscode::Base32ww,raw);
		for(int i=0;i<8;++i){
			//qDebug()<<"BYTE: "<<i<<" is "<<QString::number(raw[i]);
			qint<<=8;
			quint<<=8;
			qint|=raw[i];
			quint|=(unsigned char)raw[i];
		}
	}
}


const QString Fingerprint::getHEX() const {
	return id;
}

const QString Fingerprint::getBase32ww() const {
	return b32ww;
}


const QByteArray Fingerprint::getRaw() const{
	return raw;
}


quint64 Fingerprint::getQuint64() const{
	return quint;
}

qint64 Fingerprint::getQint64() const{
	return qint;
}


quint32 Fingerprint::getQuint32() const{
	return quint&0b11111111111111111111111111111111;
}

qint32 Fingerprint::getQint32() const{
	return qint&0b11111111111111111111111111111111;
}

bool Fingerprint::isValid()const{
	return ""!=id;
}


QString Fingerprint::toString(){
	return "id="+id+", quint="+QString::number(quint)+", qint="+QString::number(qint);
}

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
