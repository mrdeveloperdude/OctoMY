/** private imeplementation.
 * https://github.com/azadkuh/qhttp
 *
 * @author amir zamani
 * @version 2.0.0
 * @date 2014-07-11
  */

#ifndef QHTTPSERVER_PRIVATE_HPP
#define QHTTPSERVER_PRIVATE_HPP
///////////////////////////////////////////////////////////////////////////////

#include "qhttp/qhttpserver.hpp"
#include "qhttp/qhttpserverconnection.hpp"
#include "qhttp/qhttpserverrequest.hpp"
#include "qhttp/qhttpserverresponse.hpp"

#include <QTcpServer>
#include <QLocalServer>
///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace server {
///////////////////////////////////////////////////////////////////////////////

class QHttpServerPrivate
{
public:
	template<class TServer>
	class BackendServer : public TServer
	{
	public:
		QHttpServer*    iserver;

		explicit BackendServer(QHttpServer* s) : TServer(s), iserver(s) {
		}

	protected:
		// if it's a QTcpServer
		virtual void    incomingConnection(qintptr socketDescriptor) {
			iserver->incomingConnection(socketDescriptor);
		}

		// if it's a QLocalServer
		virtual void    incomingConnection(quintptr socketDescriptor) {
			iserver->incomingConnection((qintptr) socketDescriptor);
		}
	};

	typedef QScopedPointer<BackendServer<QTcpServer>>   TTcpServer;
	typedef QScopedPointer<BackendServer<QLocalServer>> TLocalServer;

public:
	quint32         itimeOut = 0;
	TServerHandler  ihandler = nullptr;

	TBackend        ibackend = ETcpSocket;

	TTcpServer      itcpServer;
	TLocalServer    ilocalServer;

public:
	explicit    QHttpServerPrivate() {
		QHTTP_LINE_DEEPLOG
	}

	virtual    ~QHttpServerPrivate() {
		QHTTP_LINE_DEEPLOG
	}

	void        initialize(TBackend backend, QHttpServer* parent) {
		ibackend = backend;

		if ( ibackend == ETcpSocket ) {
			itcpServer.reset( new BackendServer<QTcpServer>(parent) );
			ilocalServer.reset( nullptr );

		} else if ( ibackend == ELocalSocket ) {
			itcpServer.reset( nullptr );
			ilocalServer.reset( new BackendServer<QLocalServer>(parent) );
		}
	}

};

///////////////////////////////////////////////////////////////////////////////
} // namespace server
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////

#endif // QHTTPSERVER_PRIVATE_HPP
