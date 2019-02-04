#include "FlowControl.hpp"
#include <QDebug>
#include <QTextStream>

FlowControl::FlowControl():
	mMode(Bad)
	, mPenaltyTime(4000)
	, mGoodConditionsTime(0)
	, mPenaltyReductionAccumulator(0)
{
	//qDebug()<<"Flow control initialized\n";
}


FlowControl::~FlowControl()
{

}


void FlowControl::reset()
{
	mMode = Bad;
	mPenaltyTime = 4000;
	mGoodConditionsTime = 0;
	mPenaltyReductionAccumulator = 0;
}

void FlowControl::update( quint64 deltaTime, quint64 rtt )
{
	static const quint64 RTTThreshold = 250;
	switch(mMode) {
	case(Good): {
		if ( rtt > RTTThreshold ) {
			qDebug()<<"*** Comms Channel Flow Control: Dropping to bad mode ***\n";
			mMode = Bad;
			if ( mGoodConditionsTime < 10000 && mPenaltyTime < 60000 ) {
				mPenaltyTime *= 2;
				if ( mPenaltyTime > 60000) {
					mPenaltyTime = 60000;
				}
				qDebug()<<"penalty time increased to "<<mPenaltyTime;
			}
			mGoodConditionsTime = 0;
			mPenaltyReductionAccumulator = 0;
			return;
		}
		mGoodConditionsTime += deltaTime;
		mPenaltyReductionAccumulator += deltaTime;

		if ( mPenaltyReductionAccumulator > 10000 && mPenaltyTime > 1000 ) {
			mPenaltyTime /= 2;
			if ( mPenaltyTime < 1000 ) {
				mPenaltyTime = 1000;
			}
			qDebug()<<"penalty time reduced to "<< mPenaltyTime ;
			mPenaltyReductionAccumulator = 0;
		}
	}
	break;
	default:
	case(Bad): {
		if ( rtt <= RTTThreshold ) {
			mGoodConditionsTime += deltaTime;
		} else {
			mGoodConditionsTime = 0;
		}
		if ( mGoodConditionsTime > mPenaltyTime ) {
			qDebug()<< "*** Comms Channel Flow Control: upgrading to good mode ***\n";
			mGoodConditionsTime = 0;
			mPenaltyReductionAccumulator = 0;
			mMode = Good;
			return;
		}
	}
	break;
	}
}

quint64 FlowControl::sendRate()const
{
	return mMode == Good ? 30 : 10;
}

QString FlowControl::toString(QString sep) const
{
	QString out;
	QTextStream ts(&out);
	ts << "sendRate: "<<sendRate()<<sep;
	ts << "mode: "<<(Good==mMode?"GOOD":"BAD")<<sep;
	ts << "penaltyTime: "<<mPenaltyTime<<sep;
	ts << "goodTime: "<<mGoodConditionsTime<<sep;
	ts << "penaltyRed: "<<mPenaltyReductionAccumulator<<sep;
	return out;
}
