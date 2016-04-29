#include "ZooMain.hpp"

#include "../libqhttp/qhttpserver.hpp"
#include "../libqhttp/qhttpserverrequest.hpp"
#include "../libqhttp/qhttpserverresponse.hpp"

#include <QtNetwork/QHostAddress>
#include <QCoreApplication>

int main(int argc, char *argv[]){
	QCoreApplication app(argc, argv);

	using namespace qhttp::server;
	QHttpServer server(&app);
	// listening on 0.0.0.0:8080
	server.listen(QHostAddress::Any, 8080, [](QHttpRequest* req, QHttpResponse* res) {

		res->setStatusCode(qhttp::ESTATUS_OK);      // http status 200
		//res->addHeader("connection", "close");    // optional, it's the default header
		res->end("Hello World!\n");                 // the response body data
		// by "connection: close", the req and res objects will be deleted automatically.
	});

	if ( !server.isListening() ) {
		qWarning()<< "failed. can not listen at port 8080!";
		return -1;
	}

	return app.exec();}
