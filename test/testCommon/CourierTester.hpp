#ifndef COURIERTESTER_HPP
#define COURIERTESTER_HPP

#include <QObject>
#include <QList>

class Courier;
class QByteArray;

/*!
 * \brief The CourierTester class allows us to test pairs of Couriers without
 * having to start networking or implicate CommsChannel spesific details
 * while still simulating all kinds of "bad weather" on the network.
 *
 * It will eventually support all of the following network deteriorations:
 *
 * [X] Packet reordering
 * [ ] Packet bit corruption
 * [ ] Packet duplication
 * [ ] Packet drops
 *
 */

class CourierTester: public QObject
{
	Q_OBJECT
protected:
	Courier *fromCourier;
	Courier *toCourier;
	// Efforts made in sendingOpportunity and dataReceived are stored here and retreived at random to simulate the delay of network
	QList<QByteArray *> fromStreams;
	QList<QByteArray *> toStreams;
	bool done;

public:

	explicit CourierTester(Courier *fromCourier, Courier *toCourier);

	void onTestInit();
	void onTestDeInit();
	void onToReceiving();
	void onFromReceiving();
	void onToSend();
	void onFromSend();


public:

	virtual void onTestInitImp();
	virtual void onTestDeInitImp();
	virtual void onToReceivingImp();
	virtual void onFromReceivingImp();
	virtual void onToSendImp();
	virtual void onFromSendImp();
	void test();
};

#endif // COURIERTESTER_HPP
