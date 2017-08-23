#include "TestEyesWidget.hpp"

#include "expression/EyesWidget.hpp"
#include "security/PortableID.hpp"
#include "utility/Utility.hpp"

#include <QSignalSpy>

void TestEyesWidget::test(){

	PortableID pid, *pidp=&pid;
	EyesWidget ew, *ewp=&ew;
	ew.show();
	QTimer t;
	t.start(10);
	connect(&t,&QTimer::timeout, this, [=](){
		pidp->setID(utility::randomByteArray(10).toBase64());
		ewp->setPortableID(*pidp);
	});

	QSignalSpy spy(&ew, SIGNAL(close()));
	while(0==spy.count()) {
		spy.wait(100);
	}


}



QTEST_MAIN(TestEyesWidget)
