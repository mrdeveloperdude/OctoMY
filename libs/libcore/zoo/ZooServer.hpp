#ifndef ZOOSERVER_HPP
#define ZOOSERVER_HPP

#include "../libqhttp/qhttpserver.hpp"
#include "widgets/Identicon.hpp"

#include "../storage/Hashstore.hpp"
#include "comms/discovery/DiscoveryServer.hpp"


#include "PunchRegistry.hpp"
#include <QString>


class QHttpRequest;
class QHttpResponse;



class ZooServer : public qhttp::server::QHttpServer
{
	private:
		Identicon identicon;
		Hashstore storage;
		PunchRegistry punches;
		DiscoveryServer discovery;

	public:
		using QHttpServer::QHttpServer;

	public:

		explicit ZooServer();

		void start(const QString pathOrPortNumber);

	public:

		void serveFallback(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
		void serveIndex(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
		void serveIdenticon(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);
		void serveAPI(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res);

};

#endif // ZOOSERVER_HPP
