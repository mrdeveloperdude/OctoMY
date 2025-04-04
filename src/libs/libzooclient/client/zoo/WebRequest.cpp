#include "WebRequest.hpp"

#include "app/Settings.hpp"
#include "comms/NetworkOptimizer.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/time/HumanTime.hpp"
#include "utility/string/String.hpp"

#include <QDateTime>
#include <QEventLoop>


int WebRequest::sid=0;


WebRequest::WebRequest(QString friendlyName):
	id(sid++)
	, friendlyName(friendlyName+"-("+QString::number(id)+"/"+QString::number(sid)+")")
	, rep(nullptr)
	, method(UNSET_METHOD)
	, nam(NetworkOptimizer::instance())
	, lastProgress(0)
	, timer(this)
	, reqStart(0)
	, timeoutInterval(0)
	, timeoutCompleted(false)
	, completed(false)
	, httpErrorCode("")
	, wasProcessed(false)
	, doMultipart(false)
{
	OC_METHODGATE();
	reqStart=utility::time::currentMsecsSinceEpoch<qint64>();
	req.setRawHeader("User-Agent", Settings::USERAGENT.toUtf8());
}


WebRequest::~WebRequest()
{
	OC_METHODGATE();
}


//http://ynonperek.com/course/qt/web-service.html

QString methodToString(Method m)
{
#define RF_METHOD_TO_STRING_CASE(a) case(a): return #a
	switch(m) {
		RF_METHOD_TO_STRING_CASE(UNSET_METHOD);
		RF_METHOD_TO_STRING_CASE(GET);
		RF_METHOD_TO_STRING_CASE(PUT);
		RF_METHOD_TO_STRING_CASE(POST);
		RF_METHOD_TO_STRING_CASE(HEAD);
	}
#undef RF_METHOD_TO_STRING_CASE
	return "ERROR: METHOD NOT FOUND";
}


QDebug operator<<(QDebug d, Method m)
{
	OC_FUNCTIONGATE();
	d.nospace() << methodToString(m);
	return d.space();
}



QString WebRequest::elapsed()
{
	OC_METHODGATE();
	if(reqStart>0) {
		return utility::string::humanReadableElapsedSeconds((utility::time::currentMsecsSinceEpoch<qint64>()-reqStart)/static_cast<long double>(1000.0), 3);
	} else {
		return "Not started";
	}
}


QString WebRequest::signature()
{
	OC_METHODGATE();
	return "WR " +friendlyName.trimmed().toUpper()+": ";
}


void WebRequest::hookManagerSignals()
{
	OC_METHODGATE();
	if(!connect(&nam, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),this, SLOT(onManagerAuthenticationRequired(QNetworkReply *, QAuthenticator *)),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
	if(!connect(&nam, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)),this, SLOT(onManagerProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
}

void WebRequest::hookReplySignals()
{
	OC_METHODGATE();
	if(!connect(&timer, SIGNAL(timeout()),this, SLOT(onReplyTimeout()),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
	if(!connect(rep, SIGNAL(metaDataChanged()),this, SLOT(onReplyMetaDataChanged()),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
	if(!connect(rep, SIGNAL(finished()), this, SLOT(onReplyFinished()),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
	if(!connect(rep, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(onReplyError(QNetworkReply::NetworkError)),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
#ifndef QT_NO_SSL
	if(!connect(rep, SIGNAL(encrypted()),this, SLOT(onReplyEncrypted()))) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
	if(!connect(rep, SIGNAL(sslErrors(QList<QSslError>)),this, SLOT(onReplySslErrors(QList<QSslError>)),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
#endif
	if(!connect(rep, SIGNAL(uploadProgress(qint64,qint64)),this, SLOT(onReplyProgress(qint64,qint64)),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
	if(!connect(rep, SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(onReplyProgress(qint64,qint64)),OC_CONTYPE)) {
		qWarning()<< signature()<<"ERROR: Could not connect";
	}
}

void WebRequest::startTimeoutTimer()
{
	OC_METHODGATE();
	timeoutCompleted=false;
	timer.setInterval(static_cast<int>(timeoutInterval));
	timer.setSingleShot(true);
	timer.setTimerType(Qt::TimerType::PreciseTimer);
	timer.start();
}


void WebRequest::startRequestWorker()
{
	OC_METHODGATE();
	qDebug()<<signature()<<" STARTED";
	optimizeRequest();
	reqStart=utility::time::currentMsecsSinceEpoch<qint64>();
	completed=false;
	hookManagerSignals();
	switch(method) {
	case(HEAD): {
		rep=nam.head(req);
	}
	break;
	case(GET): {
		rep=nam.get(req);
		//rep->setRawHeader(ACCEPT_HEADER.toUtf8(),APPLICATION_JSON_MIME.toUtf8());
	}
	break;
	case(POST): {
		if(out.size()>0) {
			qDebug() << signature()<<  "OUT POST: " << out.data();
			req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, Constants::APPLICATION_JSON_MIME);
			rep=nam.post(req,out);
		} else {
			qWarning() << signature()<< "ERROR: no out: "<<out;
			rep=nullptr;
		}
	}
	break;
	case(PUT): {
		if(out.size()>0) {
			qDebug() << signature()<< "OUT PUT: " << out.data();
			if(doMultipart) {
				part.setBody(out);
				part.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, Constants::APPLICATION_JSON_MIME);
				multipart.append(part);
				rep=nam.put(req,&multipart);
			} else {
				req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, Constants::APPLICATION_JSON_MIME);
				rep=nam.put(req,out);
			}
		} else {
			qWarning() << signature()<< "ERROR: no out: "<<out;
			rep=nullptr;
		}
	}
	break;

	//default:
	case(UNSET_METHOD): {
		qWarning()<< signature()<< "ERROR: method unset or UNKNOWN";
		rep=nullptr;
	}
	return;
	}
	if(nullptr!=rep) {
		hookReplySignals();
		startTimeoutTimer();
	} else {
		qWarning()<< signature() << "ERROR: no reply object";
	}
}

//From http://devdays.kdab.com/wp-content/uploads/2013/11/speeding-up-your-Qt-app-with-new-QtNetwork-features.pdf
void WebRequest::optimizeRequest()
{
	OC_METHODGATE();
	req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,true);
	//req.setAttribute(QNetworkRequest::SpdyAllowedAttribute,true); //Investigate server side and test first (http://stackoverflow.com/questions/26975990/spdy-and-tomcat-8)
}


void WebRequest::startRequest(QUrl u, bool onlyHead, qint64 to)
{
	OC_METHODGATE();
	if(UNSET_METHOD!=method) {
		qWarning()<< signature() << "ERROR: WebRequOC_METHODGATEutable. Aborting second call to startRequest (was "<<method<<")";
		return;
	}
	url=u;
	req.setUrl(url);
	method=(onlyHead?HEAD:GET);
	timeoutInterval=to;
	//qDebug()<<Q_FUNC_INFO << "url="<<u<<" method="<<method;
	startRequestWorker();
}



void WebRequest::startRequest(QUrl u, const QByteArray &o, qint64 to)
{
	OC_METHODGATE();
	if(UNSET_METHOD!=method) {
		qWarning() << "ERROR: WebRequest is not mutable. Aborting second call to startRequest (was "<<method<<")";
		return;
	}
	url=u;
	out=o;
	req.setUrl(url);
	method=PUT;
	timeoutInterval=to;
	//qDebug()<<Q_FUNC_INFO << "url="<<u<<" method="<<method;
	startRequestWorker();
}



void WebRequest::startRequestPOST(QUrl u, const QByteArray &o, qint64 to)
{
	OC_METHODGATE();
	if(UNSET_METHOD!=method) {
		qWarning() << "ERROR: WebRequest is not mutable. Aborting second call to startRequestPOST (was "<<method<<")";
		return;
	}
	url=u;
	out=o;
	req.setUrl(url);
	method=POST;
	timeoutInterval=to;
	//qDebug()<<Q_FUNC_INFO << "url="<<u<<" method="<<method;
	startRequestWorker();
}


QString WebRequest::getErrString()
{
	OC_METHODGATE();
	return errorString;
}
QUrl WebRequest::getURL()
{
	OC_METHODGATE();
	return url;
}
QString WebRequest::getHTTPCode()
{
	OC_METHODGATE();
	return httpErrorCode;
}

void WebRequest::networkResponse(const QNetworkReply::NetworkError status, const QString httpCode, const QByteArray &data, bool timeoutCompleted)
{
	OC_METHODGATE();
	Q_UNUSED(status);
	Q_UNUSED(httpCode);
	Q_UNUSED(data);
	Q_UNUSED(timeoutCompleted);
}

///////////////// REPLY SLOTS

void WebRequest::onReplyTimeout()
{
	OC_METHODGATE();
	if(completed) {
		qWarning()<< signature()<<"ERROR: POTENTIAL RACE DETECTED BETWEEN TIMEOUT AND COMPLETION. SKIPPING TIMEOUT";
		return;
	}
	//Be conservative, require the full timeout to be expired
	const qint64 now=utility::time::currentMsecsSinceEpoch<qint64>();
	const qint64 left=reqStart+timeoutInterval-now;
	if(left>0) {
		//qDebug()<< "TIMEOUT - Full timeout not reached, continuing to wait";
		timer.setInterval(static_cast<int>(left));
		timer.start();
	} else {
		qDebug()<< "TIMEOUT - Full timeout completed";
		timeoutCompleted=true;
		if(nullptr != rep) {
			//NOTE: This will trigger indirect invocation of onReplyError through the following OperationAborted (5) error so no need to call anny error functions explicitly
			rep->abort();
		} else {
			qWarning()<< signature()<<"ERROR: Timeout without reply object";
			processResponse();
		}
	}
}



void WebRequest::onReplyMetaDataChanged()
{
	OC_METHODGATE();
	//qDebug()<<Q_FUNC_INFO <<elapsed();
}

void WebRequest::onReplyFinished()
{
	OC_METHODGATE();
	processResponse();
}

//NOTE: Only used for logging. See onReplyFinished for propagation of this event to children
void WebRequest::onReplyError(QNetworkReply::NetworkError)
{
	OC_METHODGATE();
	processResponse();
}



void WebRequest::processResponse()
{
	OC_METHODGATE();
	if(wasProcessed) {
		return;
	} else {
		wasProcessed=true;
	}
	if(completed) {
		qWarning()<<signature()<<"ERROR: Double complete detected";
	} else {
		completed=true;
		timer.stop();
		if(nullptr != rep) {
			QVariant statusCode=rep->attribute(QNetworkRequest::HttpStatusCodeAttribute );
			//NOTE: This is where error is propegated to layers above (children)
			httpErrorCode=statusCode.isValid()?statusCode.toString():"N/A";
			const QNetworkReply::NetworkError status=rep->error();
			const QByteArray data = rep->readAll();
			networkResponse( status, httpErrorCode, data, timeoutCompleted);

			if ( QNetworkReply::NoError!=status ) {
				const QNetworkReply::NetworkError err=rep->error();
				const QString errStr=rep->errorString();
				qWarning()<< signature()<<"ERROR: "<<errStr<<" ("<<utility::string::networkErrorToString(err)<<") HTTP Code:"<<httpErrorCode<<elapsed();
				//emit signalNetworkError(err,errStr +  httpErrorCode);
			} else {
				//NOTE: This is where success is propegated to layers above (children)
				//emit signalNetworkResponse(data);
			}

			//According to official documentation, it is application's responsibility to free the QNetworkReply object. Quoting from http://doc.qt.io/qt-5/qnetworkaccessmanager.html#details
			//Note: After the request has finished, it is the responsibility of the user to delete the QNetworkReply object at an appropriate time. Do not directly delete it inside the slot connected to finished(). You can use the deleteLater() function.
			rep->deleteLater();
		} else {
			qWarning()<< signature()<<"ERROR: 0==rep wr";
			//TODO: Produce an answer here as well
		}
	}
	qDebug()<<signature()<<" ENDED WITH " << ((nullptr == rep)?"ERROR:NO REP":utility::string::networkErrorToString(rep->error()))<<", HTTP: "<<httpErrorCode;
	rep=nullptr;
}




void WebRequest::onReplyEncrypted()
{
	OC_METHODGATE();
	//qDebug()<<Q_FUNC_INFO <<elapsed();
}

#ifndef QT_NO_SSL
void WebRequest::onReplySslErrors( QList<QSslError> errors)
{
	OC_METHODGATE();
	//qDebug()<<Q_FUNC_INFO <<elapsed();
	if(errors.count()>0) {
		qWarning()<< signature()<<"ERROR: SSL: "<<elapsed()<< signature();
		for(auto i: errors) {
			qWarning()<<" + "<<i;
		}
	}
}
#endif

void WebRequest::onReplyProgress(qint64 current,qint64 total)
{
	OC_METHODGATE();
	Q_UNUSED(current);
	Q_UNUSED(total);
	//qDebug()<<Q_FUNC_INFO << " "<<current<<"/"<<total<<" ("<<(total>0?(current)*100/(total):100)<<"%) " <<elapsed();
}


///////////////// MANAGER SLOTS


void WebRequest::onManagerProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator * )
{
	OC_METHODGATE();
	//qDebug()<<Q_FUNC_INFO <<elapsed();
}


void WebRequest::onManagerAuthenticationRequired(QNetworkReply *, QAuthenticator * )
{
	OC_METHODGATE();
	//qDebug()<<Q_FUNC_INFO <<elapsed();
}

