/** private imeplementation.
 * https://github.com/azadkuh/qhttp
 *
 * @author amir zamani
 * @version 2.0.0
 * @date 2014-07-11
  */

#ifndef QHTTPCLIENT_REQUEST_PRIVATE_HPP
#define QHTTPCLIENT_REQUEST_PRIVATE_HPP
///////////////////////////////////////////////////////////////////////////////
#include "qhttpbase.hpp"
#include "qhttp/qhttpclient.hpp"
#include "qhttp/qhttpclientrequest.hpp"

#include <QTcpSocket>

///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace client {
///////////////////////////////////////////////////////////////////////////////
class QHttpRequestPrivate : public HttpWriter<HttpRequestBase, QHttpRequestPrivate>
{
	Q_DECLARE_PUBLIC(QHttpRequest)

public:
	explicit    QHttpRequestPrivate(QHttpClient* cli, QHttpRequest* q) : q_ptr(q), iclient(cli) {
		QHTTP_LINE_DEEPLOG
	}

	virtual    ~QHttpRequestPrivate() {
		QHTTP_LINE_DEEPLOG
	}

	void        initialize() {
		iversion    = "1.1";

		isocket.ibackendType  = iclient->backendType();
		isocket.itcpSocket    = iclient->tcpSocket();
		isocket.ilocalSocket  = iclient->localSocket();
	}

	QByteArray  makeTitle();

	void        prepareHeadersToWrite();

protected:
	QHttpRequest* const  q_ptr;
	QHttpClient* const   iclient;
};

///////////////////////////////////////////////////////////////////////////////
} // namespace client
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
#endif // QHTTPCLIENT_REQUEST_PRIVATE_HPP
