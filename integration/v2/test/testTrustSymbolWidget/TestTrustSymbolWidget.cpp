#include "TestTrustSymbolWidget.hpp"

#include "widgets/pairing/TrustSymbolWidget.hpp"

#include "utility/time/HumanTime.hpp"

void TestTrustSymbolWidget::test()
{
	const quint64 stress_time = 1000 * 1; //Loop at X seconds
	TrustSymbolWidget *tsw=new TrustSymbolWidget();
	tsw->configure();
	tsw->show();


	for(int i=0; i<1000; ++i) {
		{
			const quint64 startTime = utility::time::currentMsecsSinceEpoch<quint64>();
			tsw->setTrustLevel(TRUST);
			while(utility::time::currentMsecsSinceEpoch<quint64>()-startTime < stress_time) {
				qApp->processEvents();
			}
		}
		{
			const quint64 startTime = utility::time::currentMsecsSinceEpoch<quint64>();
			tsw->setTrustLevel(IGNORE);
			while(utility::time::currentMsecsSinceEpoch<quint64>()-startTime < stress_time) {
				qApp->processEvents();
			}
		}
		{
			const quint64 startTime = utility::time::currentMsecsSinceEpoch<quint64>();
			tsw->setTrustLevel(BLOCK);
			while(utility::time::currentMsecsSinceEpoch<quint64>()-startTime < stress_time) {
				qApp->processEvents();
			}
		}
	}

}


OC_TEST_MAIN(test, TestTrustSymbolWidget)

