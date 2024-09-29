/*
*
* This file is part of QMapControl,
* an open-source cross-platform map widget
*
* Copyright (C) 2007 - 2008 Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QMapControl. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: kaiwinter@gmx.de
* Program URL   : http://qmapcontrol.sourceforge.net/
*
*/

#include "comms/NetworkOptimizer.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"


#include "MapNetwork.hpp"
#include <QUrl>
#include <QRegularExpression>
#include <QNetworkRequest>
#include <QMapIterator>
#include <QWaitCondition>

#include <QMutexLocker>
#include <QNetworkCookieJar>
#include <QHostInfo>

static const QRegularExpression re(".:.");

namespace qmapcontrol
{
MapNetwork::MapNetwork(ImageManager* parent)
	: mParent(parent)
	, mNetworkMan(OC_NEW QNetworkAccessManager)
	, mLoaded(0)
	, mNetworkActive( false )
	, mCacheEnabled(false)
{
	OC_METHODGATE();
	if(!connect(mNetworkMan, SIGNAL(finished(QNetworkReply *)), this, SLOT(requestFinished(QNetworkReply *)), OC_CONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
	mNetworkMan->setCookieJar(OC_NEW QNetworkCookieJar(mNetworkMan));
}

MapNetwork::~MapNetwork()
{
	OC_METHODGATE();
	foreach(QNetworkReply *reply, mReplyList) {
		if(reply->isRunning()) {
			reply->abort();
		}
		reply->deleteLater();
		reply = nullptr;
	}

	mNetworkMan->deleteLater();
	mNetworkMan = nullptr;
}





void MapNetwork::loadImage(const QString& host, const QString& url)
{
	OC_METHODGATE();
	QString hostName = host;
	QString portNumber = QString("80");
	//TODO: See if this regex conversion was successfull
	//OLD:	if(r.indexIn(host) >= 0) {
	if(host.indexOf(re) >= 0) {
		QStringList s = host.split(":");

		hostName = s.at(0);
		portNumber = s.at(1);
	}
	QString finalUrl = QString("http://%1:%2%3").arg(hostName).arg(portNumber).arg(url);
	QNetworkRequest request = QNetworkRequest(QUrl(finalUrl));
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,true);
	request.setAttribute(QNetworkRequest::Http2AllowedAttribute,true);
	if( mCacheEnabled ) {
		// prefer our cached version (if enabled) over fresh network query
		request.setAttribute( QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );
		// Data obtained should be saved to cache for future uses
		request.setAttribute( QNetworkRequest::CacheSaveControlAttribute, true );
	} else {
		qDebug()<<"CACHE DISABLED";
	}
	request.setRawHeader("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko) OctoMY");
	QMutexLocker lock(&mVectorMutex);
	mReplyList.append( mNetworkMan->get(request) );
	mLoadingMap.insert( finalUrl, url );
}

void MapNetwork::requestFinished(QNetworkReply *reply)
{
	OC_METHODGATE();
	if (!reply) {
		//qDebug() << "MapNetwork::requestFinished - reply no longer valid";
		return;
	}

	QVariant fromCache = reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
	//qDebug() << "page from cache?" << fromCache.toBool();

	//qDebug() << "MapNetwork::requestFinished" << reply->url().toString();
	if (reply->error() == QNetworkReply::NoError) {
		QString id = reply->url().toString();
		// check if id is in map?
		bool idInMap = false;
		QString url;
		{
			QMutexLocker lock(&mVectorMutex);
			idInMap = mLoadingMap.contains(id);
			if(idInMap) {
				url = mLoadingMap[id];
				mLoadingMap.remove(id);
			}
		}

		if (idInMap) {
			//qDebug() << "request finished for reply: " << reply << ", belongs to: " << url << endl;
			QByteArray ax;

			if (reply->bytesAvailable()>0) {
				QPixmap pm;
				ax = reply->readAll();

				if (pm.loadFromData(ax) && pm.size().width() > 1 && pm.size().height() > 1) {
					mLoaded += pm.size().width()*pm.size().height()*pm.depth()/8/1024;
					//qDebug() << "Network loaded: " << loaded << " width:" << pm.size().width() << " height:" <<pm.size().height();
					mParent->receivedImage(pm, url);
				} else {
					mParent->fetchFailed(url);
				}
			}
		}
	}

	if (loadQueueSize() == 0) {
		//qDebug () << "all loaded";
		mParent->loadingQueueEmpty();
	}

	QMutexLocker lock(&mVectorMutex);
	mReplyList.removeAll(reply);

	reply->deleteLater();
	reply = nullptr;
}

int MapNetwork::loadQueueSize() const
{
	OC_METHODGATE();
	QMutexLocker lock(&mVectorMutex);
	return mLoadingMap.size();
}


void MapNetwork::setDiskCache(QNetworkDiskCache *qCache)
{
	OC_METHODGATE();
	mCacheEnabled = (nullptr != qCache);
	if (nullptr!=mNetworkMan) {
		mNetworkMan->setCache(qCache);
	}
}


void MapNetwork::abortLoading()
{
	OC_METHODGATE();
	//qDebug() << "MapNetwork::abortLoading";
	// be sure that replyList is copied in case its modified in another thread
	QListIterator<QNetworkReply *> iter(mReplyList);
	while(iter.hasNext()) {
		QNetworkReply *reply = iter.next();
		if (reply) {
			if(reply->isRunning()) {
				reply->abort();
			}
			reply->deleteLater();
			reply = nullptr;
		}
	}
	QMutexLocker lock(&mVectorMutex);
	mReplyList.clear();
	mLoadingMap.clear();
}

bool MapNetwork::imageIsLoading(QString url)
{
	OC_METHODGATE();
	QMutexLocker lock(&mVectorMutex);
	return mLoadingMap.values().contains(url);
}

void MapNetwork::setProxy(const QString host, const quint16 port, const QString username, const QString password)
{
	OC_METHODGATE();
	// do not set proxy on qt/extended
#ifndef Q_WS_QWS
	if (mNetworkMan) {
		QNetworkProxy proxy = QNetworkProxy( QNetworkProxy::HttpProxy, host, port );
		proxy.setUser( username );
		proxy.setPassword( password );
		mNetworkMan->setProxy( proxy );
	}
#endif
}

}
