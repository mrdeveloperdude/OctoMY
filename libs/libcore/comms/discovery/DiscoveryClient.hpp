#ifndef DISCOVERYCLIENT_HPP
#define DISCOVERYCLIENT_HPP


#include "DiscoveryRole.hpp"
#include "DiscoveryClientStore.hpp"


#include <QTimer>
#include <QObject>
#include <QDateTime>
#include <QUrl>



class DiscoveryParticipant;
class Node;

namespace qhttp{
	namespace client{
		class QHttpClient;
	}
}

class DiscoveryClient: public QObject
{
		Q_OBJECT
	private:
		QTimer timer;
		quint64 lastZooPair;
		//QMap<QString, DiscoveryParticipant *> participants;
		QUrl  m_serverURL;
		qhttp::client::QHttpClient     *m_client;
		Node &node;
		const QString ourPubKey;
		const QString ourID;
		const QString zeroID;

	private:
		void registerPossibleParticipant(QVariantMap map);

	public:
		DiscoveryClient(Node &node);

	public:

		void start();
		void stop();

		void discover();

		Node &getNode();

//		QMap<QString, DiscoveryParticipant *> &getParticipants();

	private slots:

		void onTimer();

	signals:

		void nodeDiscovered(QString id);
};

#endif // DISCOVERYCLIENT_HPP
