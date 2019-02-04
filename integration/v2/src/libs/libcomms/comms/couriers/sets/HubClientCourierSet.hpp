#ifndef HUBCLIENTCOURIERSET_HPP
#define HUBCLIENTCOURIERSET_HPP

#include "comms/couriers/CourierSet.hpp"


#include <QDataStream>
#include <QSharedPointer>

class HubClient;


class HubClientCourierSet : public CourierSet
{
private:
	QSharedPointer<HubClient> mClient;
	QDataStream mDatastream;

public:
	explicit HubClientCourierSet();
	virtual ~HubClientCourierSet();

public:
	void configure(QString fullID, QSharedPointer<HubClient> client);


};


#endif
// HUBCLIENTCOURIERSET_HPP
