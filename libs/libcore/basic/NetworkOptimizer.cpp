#include "NetworkOptimizer.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QHostInfo>
#include <QNetworkDiskCache>
#include <QNetworkCookieJar>


NetworkOptimizer::NetworkOptimizer()
{

}


QNetworkAccessManager *nam=0;

QNetworkAccessManager &NetworkOptimizer::instance(QString host){
	if(0==nam){
		nam=new QNetworkAccessManager();
	}
	if(0!=nam){
		//Make HTTP cache dir
		const QString cacheDir=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
		if(""!=cacheDir){
			QDir dir(cacheDir);
			if(!dir.exists()){
				if(!dir.mkpath(cacheDir)){
					qWarning()<<"ERROR: Could not create HTTP cache dir "<<cacheDir;
				}
			}
			if (dir.exists()){
				QNetworkDiskCache *cache=new QNetworkDiskCache;
				cache->setCacheDirectory(cacheDir);
				nam->setCache(cache);
			}
			else{
				qWarning()<<"ERROR: HTTP cache dir missing: "<<dir.absolutePath();
			}
		}
		else{
			qWarning()<<"WARNING: Webcache for qnam disabled";
		}
		//Heat up DNS
		QHostInfo::lookupHost(host,0,0);
		//Set up cookie jar
		nam->setCookieJar(new QNetworkCookieJar);
		//Heat up HTTP (6 sockets of each type)
		for(int i=0;i<6;++i){
			nam->connectToHost(host,80);
#ifndef QT_NO_SSL
			nam->connectToHostEncrypted(host,443);
#endif
		}
	}
	else{
		qWarning()<<"ERROR: Could not allocate qnam";
	}
	return *nam;
}
