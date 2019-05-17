#ifndef COURIERSET_HPP
#define COURIERSET_HPP

#include "uptime/SharedPointerWrapper.hpp"

class Courier;

#include <QSet>


class CourierSet: public QSet<QSharedPointer<Courier> >
{
public:
	explicit CourierSet();
	virtual ~CourierSet();

public:
	// Register/unregister all couriers in this set to their respective comms
	// TODO: Rename to enable()
	void enableComms(bool enable);
	// Return if all (conservative) or more than one (non-conservative) couriers are registered with their respective comms in this set
	bool commsEnabled(bool conservative=true);
	// Register or unregister courier with this set
	// TODO: Rename to register()
	bool registerCourier(QSharedPointer<Courier> courier, bool reg);
};

#endif
// COURIERSET_HPP
