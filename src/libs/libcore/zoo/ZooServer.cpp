#include "ZooServer.hpp"

#include "qhttp/qhttpserver.hpp"
#include "qhttp/qhttpserverrequest.hpp"
#include "qhttp/qhttpserverresponse.hpp"
#include "qhttp/qhttpserverconnection.hpp"
#include "widgets/IdenticonWidget.hpp"
#include "template/Mustache.hpp"

#include "basic/Associate.hpp"
#include "basic/AddressEntry.hpp"
#include "utility/ScopedTimer.hpp"

#include "security/PortableID.hpp"
#include "utility/Utility.hpp"

#include "ZooConstants.hpp"

#include "node/AppContext.hpp"


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
#include <QStandardPaths>
#include <QCommandLineParser>
#include <QProcessEnvironment>


const quint64 ZooServer::BACKGROUND_TIMER_INTERVAL=1000*10; //10 sec
const quint64 ZooServer::PRUNE_DEADLINE=1000*60*5; //5 min
static const quint16 ZOO_MINIMAL_ADMIN_ID_LENGTH=10;


ZooServer::ZooServer(AppContext *context, QObject *parent)
	: QHttpServer(parent)
	, mContext(context)
	, mKeystore (mContext->baseDir() + "/keystore.json")
	, mStorage(QDir::current())

{
	OC_METHODGATE();
	Q_ASSERT(nullptr!=mContext);
	//ScopedTimer zooBootTimer(mContext->base()+"-boot");
	setObjectName(mContext->base());

	mKeystore.setHookSignals(*this, true);
	mKeystore.bootstrap();

	if(!QDir().mkpath(mContext->baseDir())) {
		qWarning()<<"ERROR: Could not create basedir for zoo";
	}

	qDebug()<<"Current dir for zoo storage is: "<<mStorage.dir().absolutePath();

	if(!connect(&mBackgroundTimer, &QTimer::timeout, this, &ZooServer::onBackgroundTimer,OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

	mBackgroundTimer.setInterval(BACKGROUND_TIMER_INTERVAL);
	mBackgroundTimer.setTimerType(Qt::CoarseTimer);

	Q_INIT_RESOURCE(zootpl);
	mAdminIndexTPL=utility::fileToString("://zootpl/admin_index.mtpl");
}


ZooServer::~ZooServer()
{
	OC_METHODGATE();
}

static QDebug operator<<(QDebug d, qhttp::server::QHttpRequest &s)
{
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

QDebug operator<<(QDebug d, qhttp::server::QHttpRequest *s)
{
	if(0!=s) {
		d <<*s;
	} else {
		d.nospace()<<"NULL";
	}
	return d.space();
}


NetworkAddress ZooServer::serverAddress()
{
	QTcpServer *tc=tcpServer();
	return (nullptr!=tc)?NetworkAddress (QHostAddress(tc->serverAddress().toString()), tc->serverPort()): NetworkAddress();
}

bool ZooServer::start(const QString pathOrPortNumber)
{
	OC_METHODGATE();
	if(!connect(this,  &QHttpServer::newConnection, [this](qhttp::server::QHttpConnection*) {
	//qDebug()<<"a new connection was made!\n";
})) {
		qWarning()<<"ERROR: Could not connect";
	}
	mBackgroundTimer.start();
	qhttp::server::TServerHandler conHandler=[this](qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res) {
		req->collectData(OC_COLLECT_AT_MOST);
		req->onEnd([this, req, res]() {
			//qDebug()<<req;
			QString path=req->url().path();
			//qDebug()<<"URL: "<<path;
			if("/"==path) {
				serveIndex(req,res);
				return;
			} else if(path.startsWith("/identicon") || path.startsWith("/favicon.ico")) {
				serveIdenticon(req,res);
				return;
			} else if(path.startsWith("/api")) {
				serveAPI(req,res);
				return;
			} else if( mAdminURL.length()>0 && path.startsWith(mAdminURL)) {
				serveAdmin(req,res);
				return;
			} else {
				serveFallback(req,res);
			}
		});
	};

	const bool isListening = listen(pathOrPortNumber, conHandler);

	if ( !isListening ) {
		qWarning()<<"ERROR: Could not listen to "<<qPrintable(pathOrPortNumber);
		//QCoreApplication::quit();
		return false;
	} else {
		qDebug()<<"Listening to "<<qPrintable(pathOrPortNumber);
	}
	return true;
}


void ZooServer::stop()
{
	OC_METHODGATE();
	if ( !isListening() ) {
		qWarning()<<"ERROR: Trying to stop server while not listeining";
	} else {
		qDebug()<<"Stopping server";
		stopListening();
	}
}

bool ZooServer::isStarted()const
{
	return isListening();
}

void ZooServer::serveFallback(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	res->addHeader("content-type","text/html; charset=utf-8");
	res->addHeaderValue("content-length", bytes.length());
	res->end(bytes);

}

void ZooServer::serveIdenticon(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	QUrlQuery query(req->url().query());
	QString id=query.queryItemValue("id");
	QByteArray idBA=0;
	if(""!=id) {
		idBA=id.toUtf8();
	}

	QString w=query.queryItemValue("w");
	quint64 wInt=64;
	if(""!=w) {
		wInt=w.toInt();
	}
	QString h=query.queryItemValue("h");
	quint64 hInt=64;
	if(""!=h) {
		hInt=h.toInt();
	}

	wInt=(wInt>1000)?1000:wInt;
	hInt=(hInt>1000)?1000:hInt;

	PortableID pid;
	pid.setType(TYPE_ZOO);
	mIdenticon.setPortableID(pid);
	QByteArray bytes;
	QBuffer buffer( &bytes );
	buffer.open( QIODevice::WriteOnly );
	QImage image=mIdenticon.image(wInt,hInt);
	if(image.isNull()) {
		qWarning()<<"ERROR:Image is null";
		return;
	} else {
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

static const QRegularExpression reOCID("^[0-9A-F]{40}$"); // trimmed 40-digit upper-case hex string
static const QRegularExpression rePunchToken("^[0-9]{5}$"); // trimmed 5-digit integer decimal string
static const QRegularExpression rePinPSK("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters
static const QRegularExpression reGeoPSK("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


void ZooServer::serveAPI(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	//qDebug()<<"API ENPOINT REACHED WITH NEW CONNECTION";
	QByteArray data=req->collectedData();
	//qDebug()<<"DATA RECEIVED: "<<data;

	QJsonParseError error;
	QJsonDocument jdoc= QJsonDocument::fromJson(data, &error);
	if(QJsonParseError::NoError!=error.error) {
		const static char KMessage[] = "Error parsing json";
		res->addHeader("connection", "close");
		res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		res->addHeaderValue("content-length", strlen(KMessage));
		res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
		res->end(KMessage);
		qWarning()<<"ERROR: could not parse json of request, closing. Message="<<error.errorString()<<", Offending data: " << data;
		return;
	}
	QVariantMap root = jdoc.toVariant().toMap();
	if ( root.isEmpty()  || !root.contains("action") ) {
		const static char KMessage[] = "Invalid json format";
		res->addHeader("connection", "close");
		res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		res->addHeaderValue("content-length", strlen(KMessage));
		res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
		res->end(KMessage);
		qWarning()<<"ERROR: no action found in json of request, closing. Offending data: " << data;

		return;
	}

	QString action=root.value("action").toString();
	//qDebug()<<"------------------ "<<action<<" ---------------------------------";

	res->setStatusCode(qhttp::ESTATUS_OK);
	QVariantMap map;
	map["status"] = "ok";

	if(ZooConstants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW==action) {
		handleDiscoveryEscrow(root,map,req,res);
	}

	else {
		map["message"] ="ERROR: Don't know how to handle action: "+action;
		map["status"] ="error";
	}


	QByteArray body  = QJsonDocument::fromVariant(map).toJson();
	res->addHeader("server", ZooConstants::OCTOMY_SERVER);
	res->addHeader("content-type","application/json; charset=utf-8");
	res->addHeader("connection", "keep-alive");
	res->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
	res->addHeaderValue("content-length", body.length());
	res->end(body);
}

void ZooServer::serveAdmin(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	//QByteArray data=req->collectedData();
	QVariantHash parameters;

	parameters["title"] = "OctoMY™ Zoo - Admin interface";
	QVariantList sessions = mDiscovery.toVariantList();


	QVariantList out;
	for(auto session:sessions)	{
		QVariantMap map;
		map["row"]=session;
		out << map;
	}

	parameters["sessions"]=out;
	Mustache::Renderer renderer;
	Mustache::QtVariantContext context(parameters);
	const QByteArray body=renderer.render(mAdminIndexTPL, &context).toUtf8();
	res->setStatusCode(qhttp::ESTATUS_OK);
	res->addHeader("server", ZooConstants::OCTOMY_SERVER);
	res->addHeader("content-type","text/html; charset=utf-8");
	res->addHeader("connection", "keep-alive");
	res->addHeaderValue("content-length", body.length());
	res->end(body);
}


void ZooServer::handleDiscoveryEscrow(QVariantMap &root, QVariantMap &map, qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	//qDebug()<<"FULL MAP IS: "<<root;
	QSharedPointer<Associate> part(OC_NEW Associate(root));
	qDebug()<<"GOT PARTICIPANT "<<part->name()<<"(type="<<DiscoveryTypeToString(part->type())<<", gender="<<part->gender()<<", id="<<part->id()<<")";
	NetworkAddress na(QHostAddress(req->remoteAddress()), req->remotePort());
	qDebug()<<"Attaching public address "<<na.toString() <<" to participant";
	QSharedPointer<AddressEntry> ae(new AddressEntry(na));
	part->addressList().add(ae);
	part->addPin(root.value("manualPin").toString());
	part->addPin(root.value("geoPin").toString());
	DiscoveryServerSession *ses=mDiscovery.request(part);
	if(nullptr!=ses) {
		map["peers"]=ses->toVariantMap();
	} else {
		res->setStatusCode(qhttp::ESTATUS_INTERNAL_SERVER_ERROR);
		map["status"] = "error";
		map["message"] = "ERROR: No session";
	}
}


void ZooServer::onBackgroundTimer()
{
	OC_METHODGATE();
	mDiscovery.prune(QDateTime::currentMSecsSinceEpoch()-PRUNE_DEADLINE);//Prune all not seen for some time
}


void ZooServer::onKeystoreReady(bool ok)
{
	OC_METHODGATE();
	mAdminURL="/"+mKeystore.localPortableID().id().left(ZOO_MINIMAL_ADMIN_ID_LENGTH);
	qDebug()<<"KEYSTORE READY! ADMIN ADRESS IS: "<<mAdminURL;
}
