#ifndef DISCOVERYSERVERSESSION_HPP
#define DISCOVERYSERVERSESSION_HPP


#include "basic/Associate.hpp"

#include <QDateTime>
#include <QMap>
#include <QVariantMap>

#include <QSharedPointer>

class DiscoveryServerSession
{
private:


	QMap<QString, QSharedPointer<Associate>> mParticipantsByID;

public:
	explicit DiscoveryServerSession();

	bool set(QSharedPointer<Associate> p);
	bool has(QString id);


	QVariantList toVariantMap();


	quint64 prune(quint64 deadline);

	quint64 count();

};
#endif // DISCOVERYSERVERSESSION_HPP
