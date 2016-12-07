#ifndef COURIERTESTER_HPP
#define COURIERTESTER_HPP

#include "../libutil/utility/Utility.hpp"
#include <QObject>
#include <QList>

class Courier;
class QByteArray;

enum CourierTesterProfileOrder {
	FIFO,LIFO,RANDOM
};

struct CourierTesterProfile {

private:
	qreal mToSendProbability;
	qreal mToReceiveProbability;
	qreal mToDropProbability;
	qreal mToCorruptProbability;
	qreal mFromSendProbability;
	qreal mFromReceiveProbability;
	qreal mFromDropProbability;
	qreal mFromCorruptProbability;
	qreal mTotal;
	quint64 mSleepTimeMin;
	quint64 mSleepTimeRange;
	bool mBetrayMandateSendActive;
	bool mBetrayMandateReceiveActive;
	CourierTesterProfileOrder mOrder;

public:
	explicit CourierTesterProfile( const qreal toSendProbability=0.25 , const qreal toReceiveProbability=0.25 , const qreal toDropProbability=0.0 , const qreal toCorruptProbability=0.0 , const qreal fromSendProbability=0.25 , const qreal fromReceiveProbability=0.25 , const qreal fromDropProbability=0.0 , const qreal fromCorruptProbability=0.0 , const quint64 sleepTimeMin =1 , const quint64 sleepTimeRange =99 , const bool betrayMandateSendActive=false , const bool betrayMandateReceiveActive=false, const CourierTesterProfileOrder order=FIFO)
		: mToSendProbability(qBound(0.0, toSendProbability, 1.0))
		, mToReceiveProbability(qBound(0.0, toReceiveProbability, 1.0))
		, mToDropProbability(qBound(0.0, toDropProbability, 1.0))
		, mToCorruptProbability(qBound(0.0, toCorruptProbability, 1.0))
		, mFromSendProbability(qBound(0.0, fromSendProbability, 1.0))
		, mFromReceiveProbability(qBound(0.0, fromReceiveProbability, 1.0))
		, mFromDropProbability(qBound(0.0, fromDropProbability, 1.0))
		, mFromCorruptProbability(qBound(0.0, fromCorruptProbability, 1.0))
		, mTotal(0.0)
		, mSleepTimeMin(sleepTimeMin)
		, mSleepTimeRange(sleepTimeRange)
		, mBetrayMandateSendActive(betrayMandateSendActive)
		, mBetrayMandateReceiveActive(betrayMandateReceiveActive)
		, mOrder(order)

	{
		mTotal=0.0;
		mTotal+=mToSendProbability;
		mTotal+=mToReceiveProbability;
		mTotal+=mToDropProbability;
		mTotal+=mToCorruptProbability;
		mTotal+=mFromSendProbability;
		mTotal+=mFromReceiveProbability;
		mTotal+=mFromDropProbability;
		mTotal+=mFromCorruptProbability;
	}
// Generators
public:
	int randomStep()
	{
		qreal chance=utility::frand()*mTotal;
		int step=0;
#define STEP_CHANCE(A) if(chance<(A)){chance-=(A); step++;}else{return step;}
		STEP_CHANCE(mToSendProbability);
		STEP_CHANCE(mToReceiveProbability);
		STEP_CHANCE(mToDropProbability);
		STEP_CHANCE(mToCorruptProbability);
		STEP_CHANCE(mFromSendProbability);
		STEP_CHANCE(mFromReceiveProbability);
		STEP_CHANCE(mFromDropProbability);
		STEP_CHANCE(mFromCorruptProbability);
#undef STEP_CHANCE
		qWarning()<<"ERROR: Could not generate random step, defaulting to "<< step;
		return step;
	}

	// Selectors
public:

	qreal toSendProbability()
	{
		return mToSendProbability;
	}

	qreal toReceiveProbability()
	{
		return mToReceiveProbability;
	}

	qreal toDropProbability()
	{
		return mToDropProbability;
	}

	qreal toCorruptProbability()
	{
		return mToCorruptProbability;
	}

	qreal fromSendProbability()
	{
		return mFromSendProbability;
	}

	qreal fromReceiveProbability()
	{
		return mFromReceiveProbability;
	}

	qreal fromDropProbability()
	{
		return mFromDropProbability;
	}

	qreal fromCorruptProbability()
	{
		return mFromCorruptProbability;
	}

	qreal total()
	{
		return mTotal;
	}

	quint64 sleepTimeMin()
	{
		return mSleepTimeMin;
	}

	quint64 sleepTimeRange()
	{
		return mSleepTimeRange;
	}

	bool betrayMandateSendActive()
	{
		return mBetrayMandateSendActive;
	}

	bool betrayMandateReceiveActive()
	{
		return mBetrayMandateReceiveActive;
	}

	CourierTesterProfileOrder order()
	{
		return mOrder;
	}

};

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
	Courier *mFromCourier;
	Courier *mToCourier;
	QString mFromName;
	QString mToName;
	// Efforts made in sendingOpportunity and dataReceived are stored here and retreived at random to simulate the delay of network
	QList<QByteArray *> mFromStreams;
	QList<QByteArray *> mToStreams;
	bool mDone;
	CourierTesterProfile mProfile;
	QStringList mChanceNames;
	int mRoundCounter;

private:

	void putTitle(int chance);
public:

	explicit CourierTester(Courier *mFromCourier, Courier *mToCourier, QString fromName, QString toName);

	void setProfile(CourierTesterProfile &p);

	void sleepRandom();


	QByteArray *fetchFromStream();
	QByteArray *fetchToStream();


	int randomStep();
	void testStep(int step);
	void testRandomSteps(int steps);

	void onTestInit();
	void onTestDeInit();
	void onTestRoundStart();
	void onTestRoundEnd();
	void onToReceiving();
	void onToSend();
	void onToDrop();
	void onToCorrupt();
	void onFromReceiving();
	void onFromSend();
	void onFromDrop();
	void onFromCorrupt();

public:

	virtual void onTestInitImp() {}
	virtual void onTestDeInitImp() {}
	virtual void onTestRoundStartImp() {}
	virtual void onTestRoundEndImp() {}
	virtual void onToReceivingImp() {}
	virtual void onFromReceivingImp() {}
	virtual void onToSendImp() {}
	virtual void onFromSendImp() {}

};

#endif // COURIERTESTER_HPP
