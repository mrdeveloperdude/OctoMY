#ifndef DISCOVERYSERVERSESSION_HPP
#define DISCOVERYSERVERSESSION_HPP

#include <QDateTime>
#include <QMap>
#include <QVariantMap>

#include <QSharedPointer>

class DiscoveryParticipant;

class DiscoveryServerSession{
	private:


		QMap<QString, QSharedPointer<DiscoveryParticipant> > mParticipantsByID;

	public:
		explicit DiscoveryServerSession();

		bool set(QSharedPointer<DiscoveryParticipant> p);


		QVariantList toVariantMap();


		quint64 prune(quint64 deadline);

		quint64 count();

};
#endif // DISCOVERYSERVERSESSION_HPP
