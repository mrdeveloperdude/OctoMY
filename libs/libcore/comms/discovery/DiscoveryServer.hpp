#ifndef DISCOVERYSERVER_HPP
#define DISCOVERYSERVER_HPP

#include "DiscoveryParticipant.hpp"
#include "DiscoveryServerSession.hpp"

#include <QSharedPointer>

class DiscoveryServer{
	private:
		QMap<QString, DiscoveryServerSession *> registry;
	public:

		DiscoveryServerSession *request(QSharedPointer<DiscoveryParticipant> part);

		void prune(quint64 deadline);


};
#endif // DISCOVERYSERVER_HPP
