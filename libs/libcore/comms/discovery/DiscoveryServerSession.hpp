#ifndef DISCOVERYSERVERSESSION_HPP
#define DISCOVERYSERVERSESSION_HPP


#include "basic/NodeAssociate.hpp"

#include <QDateTime>
#include <QMap>
#include <QVariantMap>

#include <QSharedPointer>

class DiscoveryServerSession{
	private:


		QMap<QString, QSharedPointer<NodeAssociate>> mParticipantsByID;

	public:
		explicit DiscoveryServerSession();

		bool set(QSharedPointer<NodeAssociate> p);
		bool has(QString id);


		QVariantList toVariantMap();


		quint64 prune(quint64 deadline);

		quint64 count();

};
#endif // DISCOVERYSERVERSESSION_HPP
