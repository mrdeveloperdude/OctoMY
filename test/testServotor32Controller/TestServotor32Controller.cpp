#include "TestServotor32Controller.hpp"

#include "hw/actuators/Servotor32Controller.hpp"


void TestServotor32Controller::test()
{
	Servotor32Controller ctl;
	qApp->processEvents();
	QVERIFY(!ctl.isConnected());
	ctl.configure();
	qApp->processEvents();
	QTest::qWait(2000);
	//QVERIFY(!ctl.isConnected());	ctl.openSerialPort();
	qApp->processEvents();
	QVERIFY(ctl.isConnected());
	qApp->processEvents();
	Pose pose1(2);
	Pose pose2(2);
	pose1.setValue(0,-1.0);
	pose1.setValue(1,+1.0);
	pose2.setValue(0,+1.0);
	pose2.setValue(1,-1.0);
	qDebug()<<"GO!!!";
	ctl.move(pose1);
	QTest::qWait(1000);
	ctl.move(pose2);
	QTest::qWait(1000);
	ctl.centerAll();
	QTest::qWait(1000);
}



QTEST_MAIN(TestServotor32Controller)
