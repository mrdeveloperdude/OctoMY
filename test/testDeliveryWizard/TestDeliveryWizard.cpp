#include "TestDeliveryWizard.hpp"

#include "agent/Agent.hpp"
#include "app/AppContext.hpp"
#include "app/launcher/AppLauncher.hpp"
#include "node/Node.hpp"
#include "uptime/New.hpp"
#include "delivery/AgentDeliveryActivity.hpp"

#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QSignalSpy>


class AgentDeliveryWizardTest: public AgentDeliveryWizard
{
private:
public:
	explicit AgentDeliveryWizardTest()
		: AgentDeliveryWizard(nullptr)
	{

	}

	virtual ~AgentDeliveryWizardTest() {}
};


static void testLineEdit(QLineEdit *lineEdit, QString in, QString out, const int delay=-1)
{
	QCOMPARE_NE(lineEdit, nullptr);
	QVERIFY(lineEdit->isEnabled());
	QVERIFY(lineEdit->isVisible());
	QTest::keyClick(lineEdit, Qt::Key_A, Qt::ControlModifier, delay);
	QTest::keyClick(lineEdit, Qt::Key_Delete, Qt::NoModifier, delay);
	QCOMPARE(lineEdit->text(), QString(""));
	QTest::keyClicks(lineEdit, in, Qt::NoModifier, delay);
	QCOMPARE(lineEdit->text(), QString(out));
}


// http://stackoverflow.com/questions/38421981/how-can-i-test-a-full-qt5-gui-using-qtestlib
void TestDeliveryWizard::test()
{
	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	QSharedPointer<AppCommandLineParser> opts(OC_NEW AppCommandLineParser());


	QSharedPointer<AppContext> agentContext(OC_NEW AppContext(opts, env, "testDeliveryWizard", false));
	QVERIFY(!agentContext.isNull());

	Q_INIT_RESOURCE(icons);
	Q_INIT_RESOURCE(images);
	Q_INIT_RESOURCE(qfi);
	Q_INIT_RESOURCE(3d);

	for( int i=0; i<100; ++i) {
		QSharedPointer<AppLauncher<Agent> > nodeLauncher(OC_NEW AppLauncher<Agent>);
		nodeLauncher->configure("TestAgent", 0, nullptr);

		QSharedPointer<Node> testAgent(OC_NEW Agent);
		testAgent->appConfigure(nodeLauncher);
		QVERIFY(nullptr!=testAgent);
		QFile file(testAgent->keyStore()->filename());
		if(file.exists()) {
			file.remove();
		}
		QVERIFY(!file.exists());
		// At this point we sleep to let async init complete
		// TODO: We might want to perform synchronization instead
		QTest::qWait(1000);

		AgentDeliveryWizardTest *delWiz=OC_NEW AgentDeliveryWizardTest();
		QVERIFY(nullptr!=delWiz);
		delWiz->configure(testAgent);
		delWiz->show();




		const int delay=50;
		QLineEdit *lineEditName=delWiz->findChild<QLineEdit *>("lineEditName");
		QString agentName="arnold";
		testLineEdit(lineEditName, "AarnXol1337!!#d", agentName, delay);
		QStackedWidget *stackedWidget=delWiz->findChild<QStackedWidget*>("stackedWidget");
		QVERIFY(nullptr!=stackedWidget);
		QPushButton *pushButtonOnward=delWiz->findChild<QPushButton *>("pushButtonOnward");
		QVERIFY(nullptr!=pushButtonOnward);
		QVERIFY(pushButtonOnward->isEnabled());
		QVERIFY(pushButtonOnward->isVisible());
		QCOMPARE(stackedWidget->currentWidget()->objectName(), QString("pageDelivery"));
		QSignalSpy spyStackedWidget(stackedWidget, SIGNAL(currentChanged(int)));
		QCOMPARE(spyStackedWidget.count(), 0);
		//QSignalSpy spyPushButtonOnward(pushButtonOnward , SIGNAL(clicked(bool)));
		QTest::mouseClick(pushButtonOnward, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(), delay);
		QCOMPARE(stackedWidget->currentWidget()->objectName(), QString("pageBirthInProgress"));
		QCOMPARE(spyStackedWidget.count(), 1);
		if(!spyStackedWidget.wait(static_cast<int>(AgentDeliveryWizard::MINIMUM_BIRTH_TIME*20))){
			qWarning()<<"WAIT #1 FAILED";
		}
		QCOMPARE(spyStackedWidget.count(), 2);
		QCOMPARE(stackedWidget->currentWidget()->objectName(), QString("pageDone"));
		delete delWiz;
		delWiz=nullptr;
	}
	QTest::qWait(1000);
}


OC_TEST_MAIN(test, TestDeliveryWizard)
