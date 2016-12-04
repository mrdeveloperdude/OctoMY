#include "TestAgentStateCourier.hpp"

#include "../libcore/comms/couriers/AgentStateCourier.hpp"


#include "../testCommon/CourierTester.hpp"

class AgentStateCourierTester: public CourierTester
{

private:

	int ctr;

public:
	explicit AgentStateCourierTester(QDataStream &stream)
		: CourierTester(new AgentStateCourier(nullptr),new AgentStateCourier(&stream))
		, ctr(0)
	{

	}

private:
	void updateCount()
	{
		if(ctr++>10) {
			done=true;
		}
	}

public:
	void onTestInitImp() Q_DECL_OVERRIDE {
		CourierMandate beforeMandate=fromCourier->mandate();
		qDebug()<<"before="<<beforeMandate;


	}


	void onTestDeInitImp() Q_DECL_OVERRIDE {
		updateCount();
	}



	void onToReceivingImp() Q_DECL_OVERRIDE {
		updateCount();
	}

	void onFromReceivingImp() Q_DECL_OVERRIDE {
		updateCount();
	}

	void onToSendImp() Q_DECL_OVERRIDE {
		updateCount();
	}

	void onFromSendImp() Q_DECL_OVERRIDE {
		updateCount();
	}

};



void TestAgentStateCourier::test()
{
	QDataStream stream;
	AgentStateCourierTester agetnStateTest(stream);
	agetnStateTest.test();
}





QTEST_MAIN(TestAgentStateCourier)
