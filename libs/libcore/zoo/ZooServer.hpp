#ifndef ZOOSERVER_HPP
#define ZOOSERVER_HPP

#include "../libqhttp/qhttpserver.hpp"
#include "widgets/Identicon.hpp"

#include "../storage/Hashstore.hpp"
#include "comms/discovery/DiscoveryServer.hpp"

#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"

#include "PunchRegistry.hpp"
#include <QString>
#include <QTimer>


class QHttpRequest;
class QHttpResponse;
class QCommandLineParser;
class QProcessEnvironment;



class ZooServer : public qhttp::server::QHttpServer
{
	//	Q_OBJECT
	private:

		QString base;
		QCommandLineParser &opts;
		QProcessEnvironment &env;
		Settings settings;
		QString baseDir;
		KeyStore keystore;

		Identicon identicon;
		Hashstore storage;
		PunchRegistry punches;
		DiscoveryServer discovery;

		QTimer backgroundTimer;
		static const quint64 BACKGROUND_TIMER_INTERVAL;

	public:
		using QHttpServer::QHttpServer;

	public:

		explicit ZooServer(QCommandLineParser &opts, QProcessEnvironment &env, QObject *parent=nullptr);

		virtual~ZooServer();
		void start(const QString pathOrPortNumber);

	public:

		void serveFallback(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
		void serveIndex(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
		void serveIdenticon(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
		void serveAPI(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);

	public slots:

		void onBackgroundTimer();

};

#endif // ZOOSERVER_HPP
