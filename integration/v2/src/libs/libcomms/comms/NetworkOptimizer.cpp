#include "NetworkOptimizer.hpp"

#include "uptime/MethodGate.hpp"

#include "uptime/New.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QHostInfo>
#include <QNetworkDiskCache>
#include <QNetworkCookieJar>


NetworkOptimizer::NetworkOptimizer()
{
	OC_METHODGATE();
}


QNetworkAccessManager *NetworkOptimizer::nam=nullptr;

QNetworkAccessManager &NetworkOptimizer::instance(QString host)
{
	OC_FUNCTIONGATE();
	if(nullptr==nam) {
		nam=OC_NEW QNetworkAccessManager();
	}
	if(nullptr!=nam) {
		//Make HTTP cache dir
		const QString cacheDir=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
		if(""!=cacheDir) {
			QDir dir(cacheDir);
			if(!dir.exists()) {
				if(!dir.mkpath(cacheDir)) {
					qWarning()<<"ERROR: Could not create HTTP cache dir "<<cacheDir;
				}
			}
			if (dir.exists()) {
				QNetworkDiskCache *cache=OC_NEW QNetworkDiskCache;
				cache->setCacheDirectory(dir.absoluteFilePath("NetworkHorse"));
				nam->setCache(cache);
			} else {
				qWarning()<<"ERROR: HTTP cache dir missing: "<<dir.absolutePath();
			}
		} else {
			qWarning()<<"WARNING: Webcache for qnam disabled";
		}
		//Heat up DNS
		QHostInfo::lookupHost(host, static_cast<QObject *>(nullptr), static_cast<const char *>(nullptr));
		//Set up cookie jar
		nam->setCookieJar(OC_NEW QNetworkCookieJar);
		//Heat up HTTP (6 sockets of each type)
		for(int i=0; i<6; ++i) {
			nam->connectToHost(host, 80);
#ifndef QT_NO_SSL
			nam->connectToHostEncrypted(host, 443);
#endif
		}
	} else {
		qWarning()<<"ERROR: Could not allocate qnam";
	}
	return *nam;
}
