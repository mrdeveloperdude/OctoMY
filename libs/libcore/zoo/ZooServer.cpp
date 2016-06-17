#include "ZooServer.hpp"

#include "../libqhttp/qhttpserver.hpp"
#include "../libqhttp/qhttpserverrequest.hpp"
#include "../libqhttp/qhttpserverresponse.hpp"
#include "../libqhttp/qhttpserverconnection.hpp"
#include "widgets/IdenticonWidget.hpp"
#include "web/Mustache.hpp"

#include "comms/discovery/DiscoveryParticipant.hpp"

#include "ZooConstants.hpp"

#include <QtNetwork/QHostAddress>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QBuffer>
#include <QByteArray>

#include <QImage>
#include <QPixmap>
#include <QString>

#include <QCoreApplication>
#include <QDir>

#include <QRegularExpression>
#include <QSharedPointer>

#include <QTcpServer>



ZooServer::ZooServer()
	: QHttpServer()
	, identicon(":/icons/identicon.svg")
	, storage(QDir::current())
{
	qDebug()<<"Current dir for storage is: "<<storage.dir().absolutePath();
}


static QDebug operator<<(QDebug d, qhttp::server::QHttpRequest &s){
	d.nospace() << "HTTP_VERSION: "<<s.httpVersion();
	d.nospace() << ", METHOD: "<<s.methodString();
	d.nospace() << ", REMOTE_ADDR: "<<s.remoteAddress()<<":"<<s.remotePort();
	d.nospace() << ", SUCCESSFULL: "<<s.isSuccessful();
	const qhttp::THeaderHash &headers=s.headers();
	d.nospace() << ", HEADERS("<<headers.size()<<"): ";
	for ( auto cit = headers.constBegin(); cit != headers.constEnd(); cit++) {
		d.nospace() <<" + "<<cit.key().constData()<<"="<<cit.value().constData();
	}
	const QByteArray &data=s.collectedData();
	d.nospace() << ", BODY("<<data.size()<<"): "<<data;
	return d.space();
}

static QDebug operator<<(QDebug d, qhttp::server::QHttpRequest *s){
	if(0!=s){
		d <<*s;
	}
	else{
		d.nospace()<<"NULL";
	}
	return d.space();
}


void ZooServer::start(const QString pathOrPortNumber)
{
	if(!connect(this,  &QHttpServer::newConnection, [this](qhttp::server::QHttpConnection*){
				//qDebug()<<"a new connection was made!\n";
})){
		qWarning()<<"ERROR: Could not connect";
	}


	qhttp::server::TServerHandler conHandler=[this](qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res){
		req->collectData(10000);
		req->onEnd([this, req, res](){
			qDebug()<<req;
			QString path=req->url().path();
			qDebug()<<"URL: "<<path;
			if("/"==path){
				serveIndex(req,res);
				return;
			}
			else if(path.startsWith("/identicon") || path.startsWith("/favicon.ico")){
				serveIdenticon(req,res);
				return;
			}
			else if(path.startsWith("/api")){
				serveAPI(req,res);
				return;
			}
			else{
				serveFallback(req,res);
			}
		});
	};

	const bool isListening = listen(pathOrPortNumber, conHandler);

	if ( !isListening ) {
		qWarning()<<"ERROR: Could not listen to "<<qPrintable(pathOrPortNumber);
		QCoreApplication::quit();
		return;
	}
	else {
		qDebug()<<"Listening to "<<qPrintable(pathOrPortNumber);
	}
}

void ZooServer::serveFallback(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	const static char KMessage[] = "Invalid request";
	res->addHeader("connection", "close");
	res->addHeader("server", ZooConstants::OCTOMY_SERVER);
	res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
	//res->addHeader("connection", "keep-alive");
	res->addHeaderValue("content-length", strlen(KMessage));
	res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
	res->end(KMessage);
}



void ZooServer::serveIndex(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{

	QVariantHash contact;
	contact["name"] = "John Smith";
	contact["email"] = "john.smith@gmail.com";

	QString contactTemplate = "<b>{{name}}</b> <a href=\"mailto:{{email}}\">{{email}}</a>";

	Mustache::Renderer renderer;
	Mustache::QtVariantContext context(contact);
	QString body=renderer.render(contactTemplate, &context);
	QByteArray bytes=body.toUtf8();
	res->addHeader("server", ZooConstants::OCTOMY_SERVER);
	res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
	res->addHeaderValue("content-length", bytes.length());
	res->end(bytes);

}

void ZooServer::serveIdenticon(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	QUrlQuery query(req->url().query());
	QString id=query.queryItemValue("id");
	quint64 idInt=0;
	if(""!=id){
		idInt=id.toInt();
	}

	QString w=query.queryItemValue("w");
	quint64 wInt=64;
	if(""!=w){
		wInt=w.toInt();
	}
	QString h=query.queryItemValue("h");
	quint64 hInt=64;
	if(""!=h){
		hInt=h.toInt();
	}

	wInt=(wInt>1000)?1000:wInt;
	hInt=(hInt>1000)?1000:hInt;


	identicon.setIdenticonData(idInt);
	QByteArray bytes;
	QBuffer buffer( &bytes );
	buffer.open( QIODevice::WriteOnly );
	QImage image=identicon.image(wInt,hInt);
	if(image.isNull()){
		qWarning()<<"ERROR:Image is null";
		return;
	}
	else{
		image.save( &buffer, "PNG" );
	}

	// QList<QPair<QString, QString> > items=query.queryItems(); for(QList<QPair<QString, QString> >::iterator it=items.begin(), e=items.end();it!=e;++it){ 				QPair<QString, QString> pair=*it;    }
	res->addHeader("server", ZooConstants::OCTOMY_SERVER);
	res->addHeader("content-type","image/png");
	res->addHeader("connection", "keep-alive");
	res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
	res->addHeaderValue("content-length", bytes.length());
	res->setStatusCode(qhttp::ESTATUS_OK);
	res->end(bytes);
}

const QRegularExpression reOCID("^[0-9A-F]{40}$"); // trimmed 40-digit upper-case hex string
const QRegularExpression rePunchToken("^[0-9]{5}$"); // trimmed 5-digit integer decimal string
const QRegularExpression rePinPSK("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters
const QRegularExpression reGeoPSK("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


void ZooServer::serveAPI(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	//qDebug()<<"API ENPOINT REACHED WITH NEW CONNECTION";
	QByteArray data=req->collectedData();
	//qDebug()<<"DATA RECEIVED: "<<data;



	QJsonDocument jdoc= QJsonDocument::fromJson(data);
	QVariantMap root = jdoc.toVariant().toMap();
	if ( root.isEmpty()  || !root.contains("action") ){
		const static char KMessage[] = "Invalid JSon format!";
		res->addHeader("connection", "close");
		res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		res->addHeaderValue("content-length", strlen(KMessage));
		res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
		res->end(KMessage);
		qWarning()<<"ERROR: no action found in json of request, closing";

		return;
	}

	QString action=root.value("action").toString();
	qDebug()<<"------------------ "<<action<<" ---------------------------------";

	bool ok=true;
	QString msg="Have a nice day";

	QVariantMap map;
	//map["action"] = action;
	if(ZooConstants::OCTOMY_ZOO_API_GET_NODE_CRUMB==action){
		QString ocid=root.value("ocid").toString();
		if(reOCID.match(ocid).hasMatch()){
			//qDebug()<<"GOT API CONNECTION WITH OCID="<<ocid;
			HashstoreRecord rec=storage.resolve(ocid);
			QByteArray ba=rec.get();
			QJsonDocument jdoc= QJsonDocument::fromBinaryData(ba);
			map["data"]=jdoc.toVariant().toMap();
			map["ocid"] = ocid;
		}
		else {
			qWarning()<<"ERROR: OCID did not match validation:" <<ocid;
			msg="ERROR: OCID did not match validation: "+ocid;
			ok=false;
		}

	}
	else if(ZooConstants::OCTOMY_ZOO_API_PUT_NODE_CRUMB==action){
		QString ocid=root.value("ocid").toString();
		if(reOCID.match(ocid).hasMatch()){
			//qDebug()<<"GOT API CONNECTION WITH OCID="<<ocid;
			HashstoreRecord rec=storage.resolve(ocid);
			QVariantMap data=root.value("data").toMap();
			QByteArray ba;
			QDataStream ds(ba);
			ds<<data;
			rec.put(ba);
			map["ocid"] = ocid;
		}
		else {
			qWarning()<<"ERROR: OCID did not match validation:" <<ocid;
			msg="ERROR: OCID did not match validation:" +ocid;
			ok=false;
		}
	}
	else if(ZooConstants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW==action){
		QString publicKey=root["publicKey"].toString();
		QString localAddress=root["localAddress"].toString();
		quint16 localPort=root["localPort"].toInt();
		QTcpServer *tc=tcpServer();
		QString publicAddress="";
		quint16 publicPort=root["publicPort"].toInt();
		if(nullptr!=tc){
			publicAddress=tc->serverAddress().toString();
		}
		QSharedPointer<DiscoveryParticipant> part(new DiscoveryParticipant(publicKey, publicAddress, publicPort,localAddress, localPort));
		part->addPin(root.value("manualPin").toString());
		part->addPin(root.value("geoPin").toString());
		DiscoveryServerSession *ses=discovery.request(part);
		if(nullptr!=ses){
			map["participants"]=ses->toVariantMap();
		}
		else{
			res->setStatusCode(qhttp::ESTATUS_INTERNAL_SERVER_ERROR);
			msg="ERROR: No session";
			ok=false;
		}
	}
	else if(ZooConstants::OCTOMY_ZOO_API_UDP_PUNCH==action){
		QString punchToken=root.value("punchToken").toString();
		if(rePunchToken.match(punchToken).hasMatch()){
			//qDebug()<<"GOT API CONNECTION WITH OCID="<<ocid;
			QString remoteAddress=req->remoteAddress();
			quint16 remotePort=req->remotePort();
			QString localAddress=root["localAddress"].toString();
			quint16 localPort=root["localPort"].toInt();
			punches.update(punchToken, remoteAddress,remotePort,localAddress, localPort);
			UDPPunch *punch=punches[punchToken];
			if(nullptr!=punch){
				map["data"]=punch->toVariantMap();
			}
		}
		else {
			qWarning()<<"ERROR: punchToken did not match validation:" <<punchToken;
			msg="ERROR: punchToken did not match validation:" +punchToken;
			ok=false;
		}
	}
	else{
		msg="ERROR: Don't know how to handle action: "+action;
		ok=false;
	}

	map["status"] = ok?"ok":"error";
	map["message"] = msg;

	QByteArray body  = QJsonDocument::fromVariant(map).toJson();

	res->addHeader("server", ZooConstants::OCTOMY_SERVER);
	res->addHeader("content-type","application/json");
	res->addHeader("connection", "keep-alive");
	res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
	res->addHeaderValue("content-length", body.length());
	res->setStatusCode(qhttp::ESTATUS_OK);
	res->end(body);

}

