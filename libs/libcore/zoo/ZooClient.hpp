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
		QUrl  m_serverURL;
		qhttp::client::QHttpClient     *m_client;
	public:
		explicit ZooClient(QObject *parent=0);
		virtual ~ZooClient();

	public:

		void setURL(const QUrl& server);
		void getNodeCrumb(const QString OCID, TVariantMapHandler handler);
		void putNodeCrumb(const QString OCID, const QByteArray data);
		void punchUDP(const QString punchToken);

	signals:
		void getNodeDone();
		void putNodeDone();



};



#endif // ZOOCLIENT_HPP
