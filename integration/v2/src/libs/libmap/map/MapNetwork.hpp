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

#ifndef MAPNETWORK_HPP
#define MAPNETWORK_HPP

#include "map/qMapControlGlobal.hpp"
#include <QObject>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QVector>
#include <QPixmap>
#include <QMutex>
#include "ImageManager.hpp"


/**
		@author Kai Winter <kaiwinter@gmx.de>
 */
namespace qmapcontrol
{
class ImageManager;
class QMAPCONTROL_EXPORT MapNetwork : QObject
{
	Q_OBJECT

private:
	Q_DISABLE_COPY (MapNetwork)

	ImageManager* mParent;
	QNetworkAccessManager* mNetworkMan;
	QList<QNetworkReply*> mReplyList;
	QMap<QString, QString> mLoadingMap;
	qreal mLoaded;
	mutable QMutex mVectorMutex;
	bool    mNetworkActive;
	bool    mCacheEnabled;

public:
	MapNetwork(ImageManager* mParent);
	~MapNetwork();

	void loadImage(const QString& host, const QString& url);

	/*!
	* checks if the given url is already loading
	* @param url the url of the image
	* @return boolean, if the image is already loading
	*/
	bool imageIsLoading(QString url);

	/*!
	* Aborts all current loading threads.
	* This is useful when changing the zoom-factor, though newly needed images loads faster
	*/
	void abortLoading();
	void setProxy(QString host, quint16 port, const QString username = QString(), const QString password = QString());

	/*!
	*
	* @return number of elements in the load queue
	*/
	int loadQueueSize() const;

	/*!
	*
	* @return next free http downloader thread
	*/
	QNetworkAccessManager* nextFreeHttp();

	/*!
	* sets the disk cache for each network manager
	* @param qCache the disk cache object to set
	*/
	void setDiskCache( QNetworkDiskCache* qCache );

private:
	void heatUp(QString host);


private slots:
	void requestFinished(QNetworkReply *reply);


};

}

#endif
// MAPNETWORK_HPP
