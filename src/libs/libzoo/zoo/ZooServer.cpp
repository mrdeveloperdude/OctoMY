#include "ZooServer.hpp"

#include "address/AddressEntry.hpp"
#include "address/Associate.hpp"
#include "app/AppContext.hpp"
#include "app/Constants.hpp"
#include "app/Settings.hpp"
#include "comms/address/CarrierAddressFactory.hpp"
#include "comms/address/CarrierAddressUDP.hpp"
#include "qhttp/qhttpserver.hpp"
#include "qhttp/qhttpserverconnection.hpp"
#include "qhttp/qhttpserverrequest.hpp"
#include "qhttp/qhttpserverresponse.hpp"
#include "security/PortableID.hpp"
#include "template/Mustache.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/file/File.hpp"
#include "utility/network/Network.hpp"
#include "utility/time/HumanTime.hpp"

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
#include <QTcpServer>
#include <QStandardPaths>
#include <QCommandLineParser>
#include <QProcessEnvironment>


const quint64 ZooServer::BACKGROUND_TIMER_INTERVAL=1000*10; //10 sec
const quint64 ZooServer::PRUNE_DEADLINE=1000*60*5; //5 min
static const quint16 ZOO_MINIMAL_ADMIN_ID_LENGTH=10;

static const QRegularExpression reOCID("^[0-9A-F]{40}$"); // trimmed 40-digit upper-case hex string
static const QRegularExpression rePunchToken("^[0-9]{5}$"); // trimmed 5-digit integer decimal string
static const QRegularExpression rePinPSK("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters
static const QRegularExpression reGeoPSK("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


ZooServer::ZooServer(QObject *parent)
	: QHttpServer(parent)
	, mContext(nullptr)
	, mKeyStore(OC_NEW KeyStore())
	, mStorage(QDir::current())
	, mDiscovery (true)
	, mConfigureHelper("ZooServer", true, false, false, true, false)
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(zootpl);
	Q_INIT_RESOURCE(icons);
}


ZooServer::~ZooServer()
{
	OC_METHODGATE();
}


void ZooServer::configure(QSharedPointer<AppContext> context)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mContext=(context);
		Q_ASSERT(nullptr!=mContext);
		//ScopedTimer zooBootTimer(mContext->base()+"-boot");
		setObjectName(mContext->base());
		//mKeyStore->store().setSynchronousMode(true);
		mKeyStore->configure(mContext->baseDir() + "/keystore.json", true);

		mKeyStore->activate(true, [this](const bool ok) {
			qDebug()<<"Keystore synchronized: "<<ok << " with " <<mKeyStore->store().journal();
			onKeystoreReady(ok);
		});

		if(!QDir().mkpath(mContext->baseDir())) {
			qWarning()<<"ERROR: Could not create basedir for zoo";
		}

		qDebug()<<"Current dir for zoo storage is: "<<mStorage.dir().absolutePath();

		if(!connect(&mBackgroundTimer, &QTimer::timeout, this, &ZooServer::onBackgroundTimer, OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}

		mBackgroundTimer.setInterval(BACKGROUND_TIMER_INTERVAL);
		mBackgroundTimer.setTimerType(Qt::CoarseTimer);

		mAdminIndexTPL=utility::file::fileToString("://zootpl/admin_index.mtpl");
	}
}



QSharedPointer<CarrierAddressUDP> ZooServer::serverAddress() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		auto tc = tcpServer();
		if(nullptr!=tc){
			return QSharedPointer<CarrierAddressUDP>::create(QHostAddress(tc->serverAddress().toString()), tc->serverPort());
		}
	}
	return nullptr;
}


quint16 ZooServer::serverPort() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QTcpServer *tc=tcpServer();
		return (nullptr!=tc)?tc->serverPort():0;
	}
	return 0;
}


void ZooServer::logUsefullStuff() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const QHostAddress defaultGateway=utility::network::defaultGatewayAddress();
		QList<QHostAddress> local=utility::network::allLocalCarrierAddresses();
		const QHostAddress closest=utility::network::closestAddress(local, defaultGateway);
		qDebug().noquote().nospace()<<"ADDRESSES";
		qDebug().noquote().nospace()<<" + gateway: "<<defaultGateway;
		for(QHostAddress addr:local) {
			qDebug().noquote().nospace()<<" + addr: "<<addr<<(closest==addr?" (close)":"");
		}
	}
}


void ZooServer::connectionHandler(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res){
	OC_METHODGATE();
	req->collectData(Constants::OCTOMY_WEB_COLLECT_AT_MOST);
	req->onEnd([this, req, res]() {
		qDebug()<<req;
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
		} else if( !mAdminURLPath.isEmpty() && path.startsWith(mAdminURLPath)) {
			serveAdmin(req,res);
			return;
		} else {
			serveFallback(req,res);
		}
	});
}

bool ZooServer::start(const QString pathOrPortNumber)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		OC_METHODGATE();
		if(!connect(this,  &QHttpServer::newConnection, [](qhttp::server::QHttpConnection*) {
		//qDebug()<<"a new connection was made!\n";
		})) {
			Q_UNUSED(this);
			qWarning()<<"ERROR: Could not connect";
		}
		mBackgroundTimer.start();
		qhttp::server::TServerHandler conHandler=[this](qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res) {
			connectionHandler(req, res);
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
	return false;
}


void ZooServer::stop()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if ( !isListening() ) {
			qWarning()<<"ERROR: Trying to stop server while not listeining";
		} else {
			qDebug()<<"Stopping server";
			stopListening();
		}
	}
}


bool ZooServer::isStarted() const
{
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return isListening();
	}
	return false;
}


void ZooServer::serveFallback(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	Q_UNUSED(req);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const static char KMessage[] = "Invalid request";
		res->addHeader("connection", "close");
		res->addHeader("server", Constants::OCTOMY_SERVER);
		res->addHeader(Constants::OCTOMY_API_VERSION_HEADER, Constants::OCTOMY_API_VERSION_CURRENT);
		//res->addHeader("connection", "keep-alive");
		res->addHeaderValue("content-length", strlen(KMessage));
		res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
		res->end(KMessage);
	}
}


void ZooServer::serveIndex(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	Q_UNUSED(req);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QVariantHash contact;
		contact["name"] = "John Smith";
		contact["email"] = "john.smith@gmail.com";

		QString contactTemplate = "<b>{{name}}</b> <a href=\"mailto:{{email}}\">{{email}}</a>";

		Mustache::Renderer renderer;
		Mustache::QtVariantContext context(contact);
		QString body=renderer.render(contactTemplate, &context);
		QByteArray bytes=body.toUtf8();
		res->addHeader("server", Constants::OCTOMY_SERVER);
		res->addHeader(Constants::OCTOMY_API_VERSION_HEADER, Constants::OCTOMY_API_VERSION_CURRENT);
		res->addHeader("content-type","text/html; charset=utf-8");
		res->addHeaderValue("content-length", bytes.length());
		res->end(bytes);
	}
}


void ZooServer::serveIdenticon(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QUrlQuery query(req->url().query());
		QString id=query.queryItemValue("id");
		QByteArray idBA;
		if(""!=id) {
			idBA=id.toUtf8();
		}

		QString w=query.queryItemValue("w");
		int wInt=64;
		if(""!=w) {
			wInt=w.toInt();
		}
		QString h=query.queryItemValue("h");
		int hInt=64;
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
		res->addHeader("server", Constants::OCTOMY_SERVER);
		res->addHeader("content-type","image/png");
		res->addHeader("connection", "keep-alive");
		res->addHeader(Constants::OCTOMY_API_VERSION_HEADER, Constants::OCTOMY_API_VERSION_CURRENT);
		res->addHeaderValue("content-length", bytes.length());
		res->setStatusCode(qhttp::ESTATUS_OK);
		res->end(bytes);
	}
}


void ZooServer::serveAPI(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		//qDebug()<<"API ENPOINT REACHED WITH NEW CONNECTION";
		QByteArray data=req->collectedData();
		//qDebug()<<"DATA RECEIVED: "<<data;

		QJsonParseError error;
		QJsonDocument jdoc= QJsonDocument::fromJson(data, &error);
		if(QJsonParseError::NoError!=error.error) {
			const static char KMessage[] = "Error parsing json";
			res->addHeader("connection", "close");
			res->addHeader(Constants::OCTOMY_API_VERSION_HEADER, Constants::OCTOMY_API_VERSION_CURRENT);
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
			res->addHeader(Constants::OCTOMY_API_VERSION_HEADER, Constants::OCTOMY_API_VERSION_CURRENT);
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

		if(Constants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW == action) {
			handleDiscoveryEscrow(root, map, req, res);
		}

		else {
			map["message"] = "ERROR: Don't know how to handle action: "+action;
			map["status"] = "error";
		}


		QByteArray body  = QJsonDocument::fromVariant(map).toJson();
		res->addHeader("server", Constants::OCTOMY_SERVER);
		res->addHeader("content-type","application/json; charset=utf-8");
		res->addHeader("connection", "keep-alive");
		res->addHeader(Constants::OCTOMY_API_VERSION_HEADER, Constants::OCTOMY_API_VERSION_CURRENT);
		res->addHeaderValue("content-length", body.length());
		res->end(body);
	}
}


void ZooServer::serveAdmin(qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		Q_UNUSED(req);
		//QByteArray data=req->collectedData();
		QVariantHash parameters;

		parameters["title"] = "OctoMY™ Zoo - Admin interface";
		QVariantList sessions = mDiscovery.toVariantList();


		QVariantList out;
		for(const auto &session:sessions)	{
			QVariantMap map;
			map["row"]=session;
			out << map;
		}

		parameters["sessions"]=out;
		Mustache::Renderer renderer;
		Mustache::QtVariantContext context(parameters);
		const QByteArray body=renderer.render(mAdminIndexTPL, &context).toUtf8();
		res->setStatusCode(qhttp::ESTATUS_OK);
		res->addHeader("server", Constants::OCTOMY_SERVER);
		res->addHeader("content-type","text/html; charset=utf-8");
		res->addHeader("connection", "keep-alive");
		res->addHeaderValue("content-length", body.length());
		res->end(body);
	}
}


// Part of the server API
// Handles incomming discovery escrow request
// NOTE: Byt its very nature, ZooServer deals only in UDP addresses. Any other address type will be ignored.
void ZooServer::handleDiscoveryEscrow(QVariantMap &root, QVariantMap &map, qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		//qDebug()<<"FULL MAP IS: "<<root;
		if(!root.contains("addressList")) {
			res->setStatusCode(qhttp::ESTATUS_INTERNAL_SERVER_ERROR);
			map["status"] = "error";
			map["message"] = "ERROR: No addressList";
			return;
		}
		QVariantList addressList=root["addressList"].toList();
		QList<QSharedPointer<CarrierAddress>> addresses;
		CarrierAddressFactory caf;
		for(QVariant a:addressList) {
			auto am = a.toMap();
			auto nadr = qSharedPointerDynamicCast<CarrierAddressUDP>(caf.fromMap(am));
			if(nadr.isNull()){
				qDebug() << "NULL ADDRESS SKIPPED FROM " << am;
				continue;
			}
			if(nadr->isValidIP(false, false)) {
				qDebug() << "ADDRESS FETCHED: " << nadr->toString() << " FROM " << am;
				addresses << nadr;
			} else {
				qDebug() << "INVALID ADDRESS SKIPPED: " << nadr->toString() << " FROM " << am;
			}
		}
		const quint64 now = utility::time::currentMsecsSinceEpoch<quint64>();
		auto associate = QSharedPointer<Associate>::create(root);
		if(!associate.isNull()) {
			auto key = QSharedPointer<Key> ::create(root.value("key").toMap(), true);
			if(!key.isNull()) {
				if(key->id() == associate->id()) {
					qDebug() << "GOT PARTICIPANT " << associate->name() << "(type=" << nodeTypeToString(associate->type()) << ", gender=" << associate->gender() << ", id=" << associate->id() << ", id2=" << key->id() << ", addresses=" << associate->addressList().toString() << ")";
					auto na = QSharedPointer<CarrierAddressUDP>::create(QHostAddress(req->remoteAddress()), req->remotePort());
					qDebug() << "Attaching visible address " << na->toString() << " to participant";
					associate->addressList().add(QSharedPointer<AddressEntry>(new AddressEntry(na, "public", now)));
					for(auto nad:addresses) {
						associate->addressList().add(QSharedPointer<AddressEntry>(new AddressEntry(nad, "local", now)));
					}
					associate->addPin(root.value("manualPin").toString());
					associate->addPin(root.value("geoPin").toString());
					DiscoveryServerEntry part(associate, key);
					DiscoveryServerSession *ses=mDiscovery.request(part);
					if(nullptr!=ses) {
						map["peers"]=ses->toVariantMap();
					} else {
						res->setStatusCode(qhttp::ESTATUS_INTERNAL_SERVER_ERROR);
						map["status"] = "error";
						map["message"] = "ERROR: No session";
					}
				} else {
					qWarning().noquote().nospace() << "ERROR: key id '" << key->id() << "' different from associate id '" << associate->id() << "'";
				}
			} else {
				qWarning() << "ERROR: key was null";
			}
		} else {
			qWarning() << "ERROR: associate was null";
		}
	}
}


void ZooServer::onBackgroundTimer()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		//Prune all associates that have not been seen for some time
		mDiscovery.prune(utility::time::currentMsecsSinceEpoch<quint64>() - PRUNE_DEADLINE);
	}
}


void ZooServer::onKeystoreReady(bool ok)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(ok) {
			auto key=mKeyStore->localKey();
			if(!key.isNull()) {
				auto id=key->id().left(ZOO_MINIMAL_ADMIN_ID_LENGTH);
				mAdminURLPath="/"+id;
				const QHostAddress defaultGateway=utility::network::defaultGatewayAddress();
				QList<QHostAddress> local=utility::network::allLocalCarrierAddresses();
				const QHostAddress closest=utility::network::closestAddress(local, defaultGateway);
				qDebug().nospace().noquote()<<"KEYSTORE READY. ADMIN URL PATH IS: 'http://"<<closest.toString()<<":"<< serverPort()<<mAdminURLPath<<"'";
				logUsefullStuff();
			} else {
				qWarning()<<"KEYSTORE HAD NO LOCAL KEY";
			}
		} else {
			qWarning()<<"KEYSTORE FAILED";
		}
	}
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
	if(nullptr!=s) {
		d <<*s;
	} else {
		d.nospace()<<"NULL";
	}
	return d.space();
}
