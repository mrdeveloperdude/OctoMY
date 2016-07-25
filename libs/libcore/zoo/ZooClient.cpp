#include "ZooClient.hpp"


#include "../libqhttp/qhttpclient.hpp"
#include "../libqhttp/qhttpclientrequest.hpp"
#include "../libqhttp/qhttpclientresponse.hpp"
//#include "ZooServer.hpp"

#include "basic/UniquePlatformFingerprint.hpp"


#include "ZooConstants.hpp"

#include <QJsonDocument>
#include <QByteArray>




ZooClient::ZooClient(QObject *parent)
	: QObject(parent)
	, m_client( new qhttp::client::QHttpClient)
{
}

ZooClient::~ZooClient(){
	if(0!=m_client){
		m_client->deleteLater();
		m_client=0;
	}
}


void ZooClient::setURL(const QUrl& serverURL) {
	//qDebug()<<"Setting new URL: "<<serverURL;
	m_serverURL   = serverURL;
}

void ZooClient::getNodeCrumb(QString OCID, TVariantMapHandler handler) {
	qhttp::client::TRequstHandler reqHandler= [this, OCID](qhttp::client::QHttpRequest* req){
		//qDebug()<<"Getting node by OCID:"<<OCID << " REQ";
		QVariantMap cmd;
		cmd["action"] = ZooConstants::OCTOMY_ZOO_API_GET_NODE_CRUMB;
		cmd["ocid"] = OCID;
		QByteArray body  = QJsonDocument::fromVariant(cmd).toJson();
		//qDebug()<<"SENDING RAW JSON: "<<body;
		req->addHeader("user-agent",			ZooConstants::OCTOMY_USER_AGENT);
		req->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		req->addHeader("accept",				"application/json");
		req->addHeader("content-type",			"application/json");
		req->addHeader("connection",			"keep-alive");
		req->addHeaderValue("content-length", 	body.length());
		req->end(body);
		//qDebug()<<"Getting node by OCID:"<<OCID << " REQ END";
	};

	qhttp::client::TResponseHandler resHandler=	[this, OCID, handler](qhttp::client::QHttpResponse* res) {
		//qDebug()<<"Getting node by OCID:"<<OCID << " RES";
		res->collectData(10000);
		res->onEnd([this, res, OCID, handler](){
			bool ok=true;
			//qDebug()<<"Getting node by OCID:"<<OCID<<" RES: ON END";
			QJsonDocument doc = QJsonDocument::fromJson(res->collectedData());
			QByteArray data=doc.toJson();
			QVariantMap root = QJsonDocument::fromJson(data).toVariant().toMap();
			if ( root.isEmpty() ) {
				qWarning() << "ERROR: The result is an invalid json";
				qWarning() << "ERROR: OFFENDING JSON IS: "<<data;
				ok=false;
			}
			else{
				qDebug()<<"RETURNED STATUS  WAS: "<<root.value("status")<<", MSG:  "<<root.value("message");
				ok=ok &&("ok"==root.value("status"));
			}
			//qDebug()<<"Getting node by OCID:"<<OCID<<" RES: ON END DONE";
			emit getNodeDone();
			handler(ok,root);
		});
		//qDebug()<<"Getting node by OCID:"<<OCID << " RES DONE";
	};
	m_client->request(qhttp::EHTTP_POST, m_serverURL, reqHandler, resHandler);
}




void ZooClient::putNodeCrumb(const QString OCID, const QByteArray data) {

	qhttp::client::TRequstHandler reqHandler= [this, OCID, data](qhttp::client::QHttpRequest* req){
		//qDebug()<<"Putting node by OCID:"<<OCID<<" REQ";
		QVariantMap cmd;
		cmd["action"] = ZooConstants::OCTOMY_ZOO_API_PUT_NODE_CRUMB;
		cmd["ocid"] = OCID;
		cmd["data"] = data;
		QByteArray body  = QJsonDocument::fromVariant(cmd).toJson();
		//qDebug()<<"SENDING RAW JSON: "<<body;
		req->addHeader("user-agent",			ZooConstants::OCTOMY_USER_AGENT);
		req->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		req->addHeader("accept",				"application/json");
		req->addHeader("content-type",			"application/json");
		req->addHeader("connection",			"keep-alive");
		req->addHeaderValue("content-length", 	body.length());
		req->end(body);
		//qDebug()<<"Putting node by OCID:"<<OCID<<" REQ DONE";
	};

	qhttp::client::TResponseHandler resHandler=	[this, OCID](qhttp::client::QHttpResponse* res) {
		//qDebug()<<"Putting node by OCID:"<<OCID<<" RES";
		res->collectData(10000);
		res->onEnd([this, res, OCID](){
			//qDebug()<<"Putting node by OCID:"<<OCID<<" RES: ON END";
			QJsonDocument doc = QJsonDocument::fromJson(res->collectedData());
			QByteArray data=doc.toJson();

			QVariantMap root = QJsonDocument::fromJson(data).toVariant().toMap();
			if ( root.isEmpty() ) {
				qWarning() << "ERROR: The result is an invalid json";
				qWarning() << "ERROR: OFFENDING JSON IS: "<<data;
				return;
			}
			else{
				qDebug()<<"RETURNED STATUS  WAS: "<<root.value("status")<<", MSG:  "<<root.value("message");
			}
			//qDebug()<<"Putting node by OCID:"<<OCID<<" RES: ON END DONE";
		});
		//qDebug()<<"Putting node by OCID:"<<OCID<<" RES DONE";
	};
	m_client->request(qhttp::EHTTP_POST, m_serverURL, reqHandler, resHandler);
	emit putNodeDone();
}



void ZooClient::doPairingEscrow(const QString OCID, TVariantMapHandler handler) {
	qhttp::client::TRequstHandler reqHandler= [this, OCID](qhttp::client::QHttpRequest* req){
		//qDebug()<<"Getting node by OCID:"<<OCID << " REQ";
		QVariantMap cmd;
		cmd["action"] = ZooConstants::OCTOMY_ZOO_API_GET_NODE_CRUMB;
		cmd["ocid"] = OCID;
		QByteArray body  = QJsonDocument::fromVariant(cmd).toJson();
		//qDebug()<<"SENDING RAW JSON: "<<body;
		req->addHeader("user-agent",			ZooConstants::OCTOMY_USER_AGENT);
		req->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		req->addHeader("accept",				"application/json");
		req->addHeader("content-type",			"application/json");
		req->addHeader("connection",			"keep-alive");
		req->addHeaderValue("content-length", 	body.length());
		req->end(body);
		//qDebug()<<"Getting node by OCID:"<<OCID << " REQ END";
	};

	qhttp::client::TResponseHandler resHandler=	[this, OCID, handler](qhttp::client::QHttpResponse* res) {
		//qDebug()<<"Getting node by OCID:"<<OCID << " RES";
		res->collectData(10000);
		res->onEnd([this, res, OCID, handler](){
			bool ok=true;
			//qDebug()<<"Getting node by OCID:"<<OCID<<" RES: ON END";
			QJsonDocument doc = QJsonDocument::fromJson(res->collectedData());
			QByteArray data=doc.toJson();
			QVariantMap root = QJsonDocument::fromJson(data).toVariant().toMap();
			if ( root.isEmpty() ) {
				qWarning() << "ERROR: The result is an invalid json";
				qWarning() << "ERROR: OFFENDING JSON IS: "<<data;
				ok=false;
			}
			else{
				qDebug()<<"RETURNED STATUS  WAS: "<<root.value("status")<<", MSG:  "<<root.value("message");
				ok=ok &&("ok"==root.value("status"));
			}
			//qDebug()<<"Getting node by OCID:"<<OCID<<" RES: ON END DONE";
			emit getNodeDone();
			handler(ok,root);
		});
		//qDebug()<<"Getting node by OCID:"<<OCID << " RES DONE";
	};
	m_client->request(qhttp::EHTTP_POST, m_serverURL, reqHandler, resHandler);
}




void ZooClient::punchUDP(const QString punchToken) {

	qhttp::client::TRequstHandler reqHandler= [this, punchToken](qhttp::client::QHttpRequest* req){
		//qDebug()<<"Putting node by OCID:"<<OCID<<" REQ";
		QVariantMap cmd;
		cmd["action"] = ZooConstants::OCTOMY_ZOO_API_UDP_PUNCH;
		cmd["punchToken"] = punchToken;

		QByteArray body  = QJsonDocument::fromVariant(cmd).toJson();
		//qDebug()<<"SENDING RAW JSON: "<<body;
		req->addHeader("user-agent",			ZooConstants::OCTOMY_USER_AGENT);
		req->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		req->addHeader("accept",				"application/json");
		req->addHeader("content-type",			"application/json");
		req->addHeader("connection",			"keep-alive");
		req->addHeaderValue("content-length", 	body.length());
		req->end(body);
		//qDebug()<<"Putting node by OCID:"<<OCID<<" REQ DONE";
	};

	qhttp::client::TResponseHandler resHandler=	[this, punchToken](qhttp::client::QHttpResponse* res) {
		//qDebug()<<"Putting node by OCID:"<<OCID<<" RES";
		res->collectData(10000);
		res->onEnd([this, res, punchToken](){
			//qDebug()<<"Putting node by OCID:"<<OCID<<" RES: ON END";
			QJsonDocument doc = QJsonDocument::fromJson(res->collectedData());
			QByteArray data=doc.toJson();

			QVariantMap root = QJsonDocument::fromJson(data).toVariant().toMap();
			if ( root.isEmpty() ) {
				qWarning() << "ERROR: The result is an invalid json";
				qWarning() << "ERROR: OFFENDING JSON IS: "<<data;
				return;
			}
			else{
				qDebug()<<"RETURNED STATUS  WAS: "<<root.value("status")<<", MSG:  "<<root.value("message");
			}
			//qDebug()<<"Putting node by OCID:"<<OCID<<" RES: ON END DONE";
		});
		//qDebug()<<"Putting node by OCID:"<<OCID<<" RES DONE";
	};
	m_client->request(qhttp::EHTTP_POST, m_serverURL, reqHandler, resHandler);

}



