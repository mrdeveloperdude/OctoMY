#ifndef DISCOVERYCLIENT_HPP
#define DISCOVERYCLIENT_HPP


#include "DiscoveryRole.hpp"
#include "NodeAssociateStore.hpp"
#include "basic/NodeAssociate.hpp"


#include <QTimer>
#include <QObject>
#include <QDateTime>
#include <QUrl>




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
		QUrl  m_serverURL;
		qhttp::client::QHttpClient     *m_client;
		Node &node;
		Key &key;

	private:
		void registerPossibleParticipant(QVariantMap map);

	public:
		DiscoveryClient(Node &node);

	public:

		void setURL(const QUrl&);
		void start();
		void stop();

		void discover();

		Node &getNode();

	private slots:

		void onTimer();

	signals:

		void nodeDiscovered(QString id);
};

#endif // DISCOVERYCLIENT_HPP
