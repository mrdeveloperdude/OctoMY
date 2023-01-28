#include "GoogleSitesClassicTool.hpp"

#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QDomDocument>
#include <QNetworkReply>
#include <QNetworkRequest>

GoogleSitesClassicTool::GoogleSitesClassicTool(QObject *object)
	: QObject(object)
	, reply(nullptr)
	, file(nullptr)
	, httpRequestAborted(false)
{


#ifndef QT_NO_SSL
	connect(&qnam, &QNetworkAccessManager::sslErrors,
			this, &GoogleSitesClassicTool::sslErrors);
#endif


}


void GoogleSitesClassicTool::startRequest(const QUrl &requestedUrl)
{
	qDebug()<<"Start request '"<<requestedUrl<<"'";
	url = requestedUrl;
	httpRequestAborted = false;

	reply = qnam.get(QNetworkRequest(url));
	connect(reply, &QNetworkReply::finished, this, &GoogleSitesClassicTool::httpFinished);
	connect(reply, &QIODevice::readyRead, this, &GoogleSitesClassicTool::httpReadyRead);

	connect(reply, &QNetworkReply::downloadProgress, [=](qint64 bytesReceived, qint64 bytesTotal){qDebug()<<"Received "<<bytesReceived<<" of "<<bytesTotal;});
	connect(reply, &QNetworkReply::finished, [=](){ qDebug()<<"Download finished";	});
}

void GoogleSitesClassicTool::downloadFile(QString urlSpec)
{
	qDebug()<<"Downloading file from '"<<urlSpec<<"'";
	const bool doOverwrite=true;
	if (urlSpec.isEmpty()) {
		return;
	}

	const QUrl newUrl = QUrl::fromUserInput(urlSpec);
	if (!newUrl.isValid()) {
		qWarning()<<QString("Invalid URL: %1: %2").arg(urlSpec, newUrl.errorString());
		return;
	}

	QString fileName = newUrl.fileName();
	if (fileName.isEmpty()) {
		fileName = "/tmp/default_download.txt";
	}
	QString downloadDirectory = QDir::cleanPath("/tmp");
	bool useDirectory = !downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir();
	if (useDirectory) {
		fileName.prepend(downloadDirectory + '/');
	}
	if (QFile::exists(fileName)) {
		if (!doOverwrite) {
			qDebug()<<"Skipping file '" << fileName<< "' because overwrite is disabled";
			return;
		}
		QFile::remove(fileName);
	}

	file = openFileForWrite(fileName);
	if (!file) {
		return;
	}

	// schedule the request
	startRequest(newUrl);
}

QFile *GoogleSitesClassicTool::openFileForWrite(const QString &fileName)
{
	qDebug()<<"Opening file '"<<fileName<<"' for writing";
	QScopedPointer<QFile> file(new QFile(fileName));
	if (!file->open(QIODevice::WriteOnly)) {
		qWarning()<< QString("Unable to save the file %1: %2.").arg(QDir::toNativeSeparators(fileName), file->errorString());
		return nullptr;
	}
	return file.take();
}

void GoogleSitesClassicTool::cancelDownload()
{
	qDebug()<<"Download canceled.";
	httpRequestAborted = true;
	reply->abort();
}

void GoogleSitesClassicTool::httpFinished()
{
	qDebug()<<"HTTP Finished for "<<file;
	const bool doRedirect=true;
	QFileInfo fi;
	if (file) {
		fi.setFile(file->fileName());
		file->close();
		delete file;
		file = nullptr;
	}

	if (httpRequestAborted) {
		reply->deleteLater();
		reply = nullptr;
		return;
	}

	if (reply->error()) {
		QFile::remove(fi.absoluteFilePath());
		qDebug()<<QString("Download failed:\n%1.").arg(reply->errorString());
		reply->deleteLater();
		reply = nullptr;
		return;
	} else {
		qDebug()<<"Download succeeded";
	}

	const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

	reply->deleteLater();
	reply = nullptr;

	if (!redirectionTarget.isNull()) {
		const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
		if(!doRedirect) {
			QFile::remove(fi.absoluteFilePath());
			qWarning()<<QString("Download failed:\nRedirect rejected.");
			return;
		}
		file = openFileForWrite(fi.absoluteFilePath());
		if (!file) {
			return;
		}
		startRequest(redirectedUrl);
		return;
	}

	qDebug()<<QString("Downloaded %1 bytes to %2\nin\n%3").arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath()));
}

void GoogleSitesClassicTool::httpReadyRead()
{
	qDebug()<<"HTTP ready read for "<<file;
	// this slot gets called every time the QNetworkReply has new data.
	// We read all of its new data and write it into the file.
	// That way we use less RAM than when reading it at the finished()
	// signal of the QNetworkReply
	if (file) {
		file->write(reply->readAll());
	}
}



#ifndef QT_NO_SSL
void GoogleSitesClassicTool::sslErrors(QNetworkReply *, const QList<QSslError> &errors)
{
	QString errorString;
	foreach (const QSslError &error, errors) {
		if (!errorString.isEmpty()) {
			errorString += '\n';
		}
		errorString += error.errorString();
	}
	qWarning()<< QString("One or more SSL errors has occurred:\n%1").arg(errorString);
	reply->ignoreSslErrors();
}
#endif


void GoogleSitesClassicTool::scavenge()
{
	const QString baseUrl("https://sites.google.com/feeds/content/octomy.org/octomy");
	downloadFile(baseUrl);
	return;
	QString fileName="";



	if (fileName.isEmpty()) {
		return;
	}

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qWarning()<<QString("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString());
		return;
	}

	read(&file);
}


void GoogleSitesClassicTool::read(QIODevice *device)
{
	QDomDocument domDocument;
	QString errorStr;
	int errorLine=0;
	int errorColumn=0;
	QString versionAttribute("version");
	QString folderElement("folder");

	if (!domDocument.setContent(device, true, &errorStr, &errorLine,
								&errorColumn)) {


		qDebug()<<QString("Parse error at line %1, column %2:\n%3")
				.arg(errorLine)
				.arg(errorColumn)
				.arg(errorStr);
		return;
	}

	QDomElement root = domDocument.documentElement();
	if (root.tagName() != "xbel") {
		qWarning()<<QString("The file is not an XBEL file.");
		return;
	} else if (root.hasAttribute(versionAttribute)
			   && root.attribute(versionAttribute) != QLatin1String("1.0")) {
		qWarning()<<QString("The file is not an XBEL version 1.0 file.");
		return;
	}

	QDomElement child = root.firstChildElement(folderElement);
}
