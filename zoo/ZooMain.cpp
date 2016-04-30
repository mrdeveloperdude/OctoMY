#include "ZooMain.hpp"

#include "../libqhttp/qhttpserver.hpp"
#include "../libqhttp/qhttpserverrequest.hpp"
#include "../libqhttp/qhttpserverresponse.hpp"
#include "../libqhttp/qhttpserverconnection.hpp"
#include "../libcore/widgets/IdenticonWidget.hpp"

#include <QtNetwork/QHostAddress>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QBuffer>
#include <QByteArray>


using namespace qhttp::server;

class Server : public QHttpServer
{
	private:
		Identicon identicon;
	public:
		using QHttpServer::QHttpServer;

	public:

		explicit Server()
			: QHttpServer()
			, identicon(":/icons/identicon.svg")
		{


		}

		void start(const QString pathOrPortNumber)
		{
			connect(this,  &QHttpServer::newConnection, [this](QHttpConnection*){
				qDebug()<<"a new connection was made!\n";
			});

			bool isListening = listen(pathOrPortNumber,
									  [this](QHttpRequest* req, QHttpResponse* res){
					if ( req->headers().keyHasValue("command", "quit") ) {
					qDebug()<<"a client sends a quit command.\nserver quits.\n";
					QCoreApplication::quit();
					return;
		}
					QString path=req->url().path();
					qDebug()<<"URL: "<<path;
					if(path.startsWith("/identicon")){
					QUrlQuery query(req->url().query());
					QString id=query.queryItemValue("id");
					quint64 idInt=0;
					if(""!=id){
					idInt=id.toInt();
		}
					identicon.setIdenticonData(idInt);
					QByteArray bytes;
					QBuffer buffer( &bytes );
					buffer.open( QIODevice::WriteOnly );
					QImage image=identicon.image();
					if(image.isNull()){
					qWarning()<<"ERROR:Image is null";
					return;
		}
					else{
					image.save( &buffer, "PNG" );
		}


					// QList<QPair<QString, QString> > items=query.queryItems(); for(QList<QPair<QString, QString> >::iterator it=items.begin(), e=items.end();it!=e;++it){ 				QPair<QString, QString> pair=*it;    }
					res->addHeader("content-type","image/png");
					res->addHeader("connection", "keep-alive");
					res->addHeaderValue("content-length", bytes.length());
					res->setStatusCode(qhttp::ESTATUS_OK);
					res->end(bytes);
					return;
		}

					req->collectData(512);
					req->onEnd([this, req, res](){
				onData(req, res);
			});
		});

			if ( !isListening ) {
				qDebug("can not listen on %s!\n", qPrintable(pathOrPortNumber));
				QCoreApplication::quit();
				return;
			}
		}

		void onData(QHttpRequest* req, QHttpResponse* res)
		{
			QVariantMap root = QJsonDocument::fromJson(req->collectedData())
					.toVariant().toMap();
			if ( root.isEmpty()  || root.value("name").toString() != QLatin1Literal("add") ) {
				const static char KMessage[] = "Invalid JSon format!";
				res->addHeader("connection", "close");
				res->addHeaderValue("content-length", strlen(KMessage));
				res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
				res->end(KMessage);
				return;
			}

			QVariantList args = root.value("arguments").toList();
			int total = 0;
			foreach (const QVariant& v, args) {
				total += v.toInt();
			}
			root["arguments"] = total;

			QByteArray body = QJsonDocument::fromVariant(root).toJson();
			res->addHeader("connection", "keep-alive");
			res->addHeaderValue("content-length", body.length());
			res->setStatusCode(qhttp::ESTATUS_OK);
			res->end(body);

		}

};


int main(int argc, char *argv[]){
	QCoreApplication app(argc, argv);

	//Q_INIT_RESOURCE(style);
	//Q_INIT_RESOURCE(fonts);
	Q_INIT_RESOURCE(icons);
	//Q_INIT_RESOURCE(qfi);
	//Q_INIT_RESOURCE(3d);

	Server server;
	server.start("8080");

	return app.exec();}
