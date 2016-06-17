#ifndef DISCOVERYCLIENT_HPP
#define DISCOVERYCLIENT_HPP

#include <QTimer>
#include <QObject>
#include <QDateTime>
#include <QUrl>

enum DiscoveryRole{
	ROLE_AGENT
	, ROLE_CONTROL
};


class DiscoveryParticipant;

namespace qhttp{
	namespace client{
		class QHttpClient;
	}
}

class DiscoveryClient: public QObject
{
		Q_OBJECT
	private:
		DiscoveryRole role;
		QTimer timer;
		quint64 lastZooPair;
		DiscoveryParticipant *zooSession;

		QUrl  m_serverURL;
		qhttp::client::QHttpClient     *m_client;

	public:
		DiscoveryClient(QObject *parent=0);

	public:

		void start();
		void stop();
		void setRole(DiscoveryRole);

		void discover();

	private slots:

		void onTimer();
};

#endif // DISCOVERYCLIENT_HPP
