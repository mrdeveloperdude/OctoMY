#ifndef DISCOVERYPARTICIPANT_HPP
#define DISCOVERYPARTICIPANT_HPP

#include <QString>
#include <QVariantMap>

struct DiscoveryParticipant{

		QString publicKey;
		QString ID;
		QStringList pins;

		QString publicAddress;
		quint16 publicPort;
		QString localAddress;
		quint16 localPort;
		quint64 lastTime;

		DiscoveryParticipant();

		DiscoveryParticipant(QString publicKey, QString publicAddress, quint16 publicPort, QString localAddress, quint16 localPort);

		DiscoveryParticipant(const DiscoveryParticipant &o);

		void addPin(QString pin);

void generateID();

		bool operator==(const DiscoveryParticipant &o) const;

		bool isSet();

		QVariantMap toVariantMap();

};


#endif // DISCOVERYPARTICIPANT_HPP
