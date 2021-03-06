#ifndef ZOOCLIENT_HPP
#define ZOOCLIENT_HPP

#include <functional>

#include <QString>
#include <QObject>
#include <QUrl>


#include <QVariantMap>


typedef std::function<void (bool, QVariantMap)>    TVariantMapHandler;

class QByteArray;

namespace qhttp{
	namespace client{
		class QHttpClient;
	}
}



class ZooClient: public QObject
{
Q_OBJECT

	public:
		QUrl  mServerURL;
		qhttp::client::QHttpClient     *mClient;
	public:
		explicit ZooClient(QObject *parent=0);
		virtual ~ZooClient();

	public:
		void setURL(const QUrl& server);
		void doPairingEscrow(const QString OCID, TVariantMapHandler handler);

	signals:
		void getNodeDone();

};



#endif // ZOOCLIENT_HPP
