
#ifndef WEBREQUEST_HPP
#define WEBREQUEST_HPP

#include "../libutil/utility/Utility.hpp"
#include "../libutil/utility/Standard.hpp"

#include <QDebug>
#include <QObject>
#include <QByteArray>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>

#include <QString>

#include <QUrl>
#include <QTimer>

class QNetworkReply;


enum Method{
	UNSET_METHOD,GET,PUT,POST,HEAD
};

//This class wraps QNetworkRequest and adds the following to it:

// + Default error-handling
// + Possibility of timeout
// + Some convenience wrappers to quickly start HEAD, POST and GET

class WebRequest:public QObject{
		Q_OBJECT
	private:
		static int sid;
	private:
		const int id;
		const QString friendlyName;
		QUrl url;
		QByteArray out;
		QByteArray in;
		QNetworkRequest req;
		QNetworkReply *rep;
		Method method;
		QNetworkAccessManager &nam;
		qint64 lastProgress;
		QTimer timer;
		qint64 reqStart;
		qint64 timeoutInterval;
		bool timeoutCompleted;
		QHttpPart part;
		QHttpMultiPart multipart;
		bool completed;
		QString httpErrorCode;
		QString errorString;
		bool wasProcessed;
		bool doMultipart;

	public:
		explicit WebRequest(QString friendlyName="Unnamed");
		virtual ~WebRequest();

	protected:
		//TODO: make get/put/post/head methods instead
		void startRequest(QUrl url, const QByteArray &out, qint64 to=WEB_TIMEOUT);
		void startRequestPOST(QUrl url, const QByteArray &out, qint64 to=WEB_TIMEOUT);
		void startRequest(QUrl url, bool onlyHead=false, qint64 to=WEB_TIMEOUT);

	protected:
		QString getErrString();
		QUrl getURL();
		QString getHTTPCode();
		QString methodToString(Method m);
		QString elapsed();
		QString signature();


	private:
		void startRequestWorker();
		void hookManagerSignals();
		void hookReplySignals();
		void startTimeoutTimer();
		void optimizeRequest();


		//WebRequest API
	protected:
		virtual void networkResponse(const QNetworkReply::NetworkError status, const QString httpCode, const QByteArray &data, bool timeoutCompleted);


	private:
		//NOTE: Merged onReplyError and onReplyFinished into one call "processRequest" to make sure it is reached for every request.
		//Why? Because official documentation for QNetworkReply( http://doc.qt.io/qt-5/qnetworkreply.html#error-2 )
		//States the following:
		//  - "This signal [error] is emitted when the reply detects an error in processing. The finished() signal will ***probably*** follow, indicating that the connection is over."
		//In other word the finish signal is **not** guaranteed after an error signal
		void processResponse();

	signals:
		//void signalNetworkResponse( const QByteArray &data );
		//	void signalNetworkError( QNetworkReply::NetworkError err, QString errString);

	private slots:

		///////////////// REPLY SLOTS
		void onReplyTimeout();
		void onReplyMetaDataChanged();
		void onReplyFinished();
		void onReplyError(QNetworkReply::NetworkError);
		void onReplyEncrypted();
#ifndef QT_NO_SSL
		void onReplySslErrors( QList<QSslError> errors);
#endif
		void onReplyProgress(qint64 current,qint64 total);

		///////////////// MANAGER SLOTS (Mostly used for preemptive logging for debugging purposes)
		void onManagerProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
		void onManagerAuthenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator);
		void onManagerNetworkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);

};




#endif // WEBREQUEST_HPP
