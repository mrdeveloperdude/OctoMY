#ifndef GOOGLESITESCLASSICTOOL_HPP
#define GOOGLESITESCLASSICTOOL_HPP

#include <QObject>
#include <QNetworkAccessManager>

class QIODevice;
class QUrl;
class QNetworkReply;
class QFile;


class GoogleSitesClassicTool: public QObject
{
	Q_OBJECT

private:

	QUrl url;
	QNetworkAccessManager qnam;
	QNetworkReply *reply;
	QFile *file;
	bool httpRequestAborted;
public:
	GoogleSitesClassicTool(QObject *object=nullptr);

public:


	void startRequest(const QUrl &requestedUrl);
private:

	QFile *openFileForWrite(const QString &fileName);

private slots:
	void downloadFile(QString urlSpec);
	void cancelDownload();
	void httpFinished();
	void httpReadyRead();
#ifndef QT_NO_SSL
	void sslErrors(QNetworkReply *, const QList<QSslError> &errors);
#endif

public:

	void scavenge();
	void read(QIODevice *device);
};

#endif // GOOGLESITESCLASSICTOOL_HPP
