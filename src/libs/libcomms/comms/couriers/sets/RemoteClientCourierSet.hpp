#ifndef REMOTECLIENTCOURIERSET_HPP
#define REMOTECLIENTCOURIERSET_HPP

#include "comms/couriers/CourierSet.hpp"


#include <QDataStream>
#include "uptime/SharedPointerWrapper.hpp"

class RemoteClient;


class RemoteClientCourierSet : public CourierSet
{
private:
	RemoteClient &mClient;
	QDataStream mDatastream;

public:
	explicit RemoteClientCourierSet(QString fullID, RemoteClient &client);
	virtual ~RemoteClientCourierSet();


};

#endif
// REMOTECLIENTCOURIERSET_HPP
