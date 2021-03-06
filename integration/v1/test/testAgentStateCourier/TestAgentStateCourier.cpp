#include "TestAgentStateCourier.hpp"

#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include "discovery/AddressBook.hpp"

#include "security/KeyStore.hpp"


#include "../common/CourierTester.hpp"


#include <QDebug>

class AgentStateCourierTester: public CourierTester
{

private:
	int ctr;
	AgentStateCourier *mFromAgentStateCourier;
	AgentStateCourier *mToAgentStateCourier;


public:
	explicit AgentStateCourierTester(QDataStream &stream, CommsChannel &comms)
		: CourierTester(OC_NEW AgentStateCourier(&stream, comms), OC_NEW AgentStateCourier(nullptr, comms), "AGENT ", "REMOTE")
		, ctr(0)
		, mFromAgentStateCourier((AgentStateCourier *)mFromCourier)
		, mToAgentStateCourier((AgentStateCourier *)mToCourier)
	{

	}

public:
	void putState()
	{

		if(ctr++>10) {
			//	mDone=true;
		}
		qDebug()<<mFromName<<*((AgentStateCourier *)mFromCourier);
		qDebug()<<mToName<<*((AgentStateCourier *)mToCourier);
	}


public:

	void setAgentRandomFlags()
	{
		quint8 rf=qrand()%0xFF;
		qDebug()<<QString("Setting random flags: 0b%1 (0x%2, %3)").arg(rf,0, 2).arg(rf,0, 16).arg(rf);
		mFromAgentStateCourier->setFlags(rf);
	}

	void setAgentMode(AgentMode mode)
	{
		qDebug()<<"Setting mode: "<<mode;
		mFromAgentStateCourier->setMode(mode);
	}

	// CourierTester interface
public:
	void onTestInitImp() Q_DECL_OVERRIDE {
		CourierMandate beforeMandate=mFromCourier->mandate();
		qDebug()<<"before="<<beforeMandate.toString();
		putState();
	}


	void onTestDeInitImp() Q_DECL_OVERRIDE {
		putState();
	}

	void onTestRoundStartImp() Q_DECL_OVERRIDE {
	}



	void onTestRoundEndImp() Q_DECL_OVERRIDE {
		putState();
	}


	void onToReceivingImp() Q_DECL_OVERRIDE {
		putState();

	}

	void onFromReceivingImp() Q_DECL_OVERRIDE {
		putState();
	}

	void onToSendImp() Q_DECL_OVERRIDE {
		putState();
	}

	void onFromSendImp() Q_DECL_OVERRIDE {
		putState();
	}

};



void TestAgentStateCourier::test()
{
	QDataStream stream;
	KeyStore keystore;
	CommsCarrierUDP carrier;
	AddressBook peers;
	CommsChannel comms(carrier, keystore, peers);
	AgentStateCourierTester agetnStateTest(stream, comms);
	// Start
	agetnStateTest.onTestInit();
	agetnStateTest.testStep(0);
	qDebug()<< "#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=";
	agetnStateTest.setAgentRandomFlags();
	agetnStateTest.setAgentMode(COMPLETE_AUTONOMY);
	agetnStateTest.onTestRoundEnd();
	qDebug()<< "#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=";
	agetnStateTest.testStep(0);
	agetnStateTest.testStep(5);
	agetnStateTest.testStep(5);
	// Done
	agetnStateTest.onTestDeInit();
}





OC_TEST_MAIN(test, TestAgentStateCourier)
