#ifndef DISCOVERYSERVERSESSION_HPP
#define DISCOVERYSERVERSESSION_HPP

#include <QDateTime>
#include <QMap>
#include <QVariantMap>

#include <QSharedPointer>

class DiscoveryParticipant;

struct DiscoveryServerSession{

		QMap<QString, QSharedPointer<DiscoveryParticipant> > participants;

		DiscoveryServerSession();

		bool set(QSharedPointer<DiscoveryParticipant> p);


		QVariantList toVariantMap();

};
#endif // DISCOVERYSERVERSESSION_HPP
