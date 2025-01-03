#ifndef ZOOSERVER_HPP
#define ZOOSERVER_HPP

#include "qhttp/qhttpserver.hpp"
#include "identity/Identicon.hpp"

#include "storage/Hashstore.hpp"
#include "discovery/DiscoveryServer.hpp"

#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"
#include "zoo/PunchRegistry.hpp"

#include <QString>
#include <QTimer>


class QHttpRequest;
class QHttpResponse;
class QCommandLineParser;
class QProcessEnvironment;

class AppContext;


class ZooServer : public qhttp::server::QHttpServer
{
	Q_OBJECT
private:

	AppContext *mContext;
	KeyStore mKeyStore;

	Identicon mIdenticon;
	Hashstore mStorage;
	PunchRegistry mPunches;
	DiscoveryServer mDiscovery;
	// Absolute URL path of admin functions
	QString mAdminURLPath;

	QTimer mBackgroundTimer;

	QString mAdminIndexTPL;
	static const quint64 BACKGROUND_TIMER_INTERVAL;
	static const quint64 PRUNE_DEADLINE;



public:
	using QHttpServer::QHttpServer;

public:

	explicit ZooServer( AppContext *context, QObject *parent=nullptr);

	virtual~ZooServer();


private:

	NetworkAddress serverAddress();
	void logUsefullStuff();

public:
	bool start(const QString pathOrPortNumber);
	void stop();
	bool isStarted() const;

public:

	void serveFallback(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
	void serveIndex(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
	void serveIdenticon(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
	void serveAPI(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
	void serveAdmin(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);

	void handleDiscoveryEscrow(QVariantMap &root, QVariantMap &map, qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);

public slots:

	void onBackgroundTimer();

	void onKeystoreReady(bool ok);

};


QDebug operator<<(QDebug d, qhttp::server::QHttpRequest *s);

#endif // ZOOSERVER_HPP
