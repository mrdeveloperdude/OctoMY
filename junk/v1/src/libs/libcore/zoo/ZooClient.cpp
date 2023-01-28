#include "ZooClient.hpp"

#include "utility/Standard.hpp"

#include "qhttp/qhttpclient.hpp"
#include "qhttp/qhttpclientrequest.hpp"
#include "qhttp/qhttpclientresponse.hpp"
//#include "ZooServer.hpp"

#include "basic/UniquePlatformFingerprint.hpp"


#include "ZooConstants.hpp"

#include <QJsonDocument>
#include <QByteArray>




ZooClient::ZooClient(QObject *parent)
	: QObject(parent)
	, mClient( OC_NEW qhttp::client::QHttpClient)
{
}

ZooClient::~ZooClient(){
	if(nullptr!=mClient){
		mClient->deleteLater();
		mClient=nullptr;
	}
}


void ZooClient::setURL(const QUrl& serverURL) {
	//qDebug()<<"Setting new URL: "<<serverURL;
	mServerURL   = serverURL;
}


void ZooClient::doPairingEscrow(const QString OCID, TVariantMapHandler handler) {
	qhttp::client::TRequstHandler reqHandler= [this, OCID](qhttp::client::QHttpRequest* req){
		qDebug()<<"doPairingEscrow handler for " << mServerURL;
		QVariantMap cmd;
		cmd["action"] = ZooConstants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW;
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
		res->collectData(OC_COLLECT_AT_MOST);
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
	mClient->request(qhttp::EHTTP_POST, mServerURL, reqHandler, resHandler);
}

