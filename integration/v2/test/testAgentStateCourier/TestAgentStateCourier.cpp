#include "TestAgentStateCourier.hpp"

#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include "discovery/AddressBook.hpp"

#include "security/KeyStore.hpp"


#include "../common_test/CourierTester.hpp"


#include <QDebug>
#include <QSharedPointer>

class AgentStateCourierTester: public CourierTester
{

private:
	int ctr;
	AgentStateCourier *mFromAgentStateCourier;
	AgentStateCourier *mToAgentStateCourier;

public:
	explicit AgentStateCourierTester(QDataStream &stream, QSharedPointer<CommsChannel> comms)
		: CourierTester(OC_NEW AgentStateCourier(&stream, comms), OC_NEW AgentStateCourier(nullptr, comms), "AGENT ", "REMOTE")
		, ctr(0)
		, mFromAgentStateCourier(static_cast<AgentStateCourier *>(mFromCourier))
		, mToAgentStateCourier(static_cast<AgentStateCourier *>(mToCourier))
	{

	}

public:
	void putState()
	{
		if(ctr++>10) {
			//	mDone=true;
		}
		qDebug()<<mFromName<<*(static_cast<AgentStateCourier *>(mFromCourier));
		qDebug()<<mToName<<*(static_cast<AgentStateCourier *>(mToCourier));
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
	//KeyStore keystore;
	//CommsCarrierUDP carrier;
	//AddressBook peers;

	QSharedPointer<CommsCarrierUDP> carrier(OC_NEW CommsCarrierUDP());
	QSharedPointer<KeyStore> keystore(OC_NEW KeyStore());
	QSharedPointer<AddressBook> peers(OC_NEW AddressBook());

	QSharedPointer<CommsChannel> comms(OC_NEW CommsChannel());
	comms->configure(carrier, keystore, peers);
	AgentStateCourierTester agentStateTest(stream, comms);
	// Start
	agentStateTest.onTestInit();
	agentStateTest.testStep(0);
	qDebug()<< "#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=";
	agentStateTest.setAgentRandomFlags();
	agentStateTest.setAgentMode(COMPLETE_AUTONOMY);
	agentStateTest.onTestRoundEnd();
	qDebug()<< "#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=#-#=";
	agentStateTest.testStep(0);
	agentStateTest.testStep(5);
	agentStateTest.testStep(5);
	// Done
	agentStateTest.onTestDeInit();
}





OC_TEST_MAIN(test, TestAgentStateCourier)
