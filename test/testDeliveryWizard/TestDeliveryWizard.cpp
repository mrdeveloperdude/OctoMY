#include "TestDeliveryWizard.hpp"

#include "basic/Node.hpp"
#include "widgets/AgentDeliveryWizard.hpp"

#include "basic/AppContext.hpp"

#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QSignalSpy>


class AgentDeliveryWizardTest: public AgentDeliveryWizard{

	public:

		explicit AgentDeliveryWizardTest()
			: AgentDeliveryWizard(nullptr)
		{

		}

};

static void testLineEdit(QLineEdit *lineEdit, QString in, QString out, const int delay=-1)
{
	QVERIFY(nullptr!=lineEdit);
	QVERIFY(lineEdit->isEnabled());
	QVERIFY(lineEdit->isVisible());
	QTest::keyClick(lineEdit, Qt::Key_A, Qt::ControlModifier, delay);
	QTest::keyClick(lineEdit, Qt::Key_Delete, Qt::NoModifier, delay);
	QCOMPARE(lineEdit->text(), QString(""));
	QTest::keyClicks(lineEdit, in, Qt::NoModifier, delay);
	QCOMPARE(lineEdit->text(), QString(out));
}

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
// http://stackoverflow.com/questions/38421981/how-can-i-test-a-full-qt5-gui-using-qtestlib
void TestDeliveryWizard::test(){
	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	QCommandLineParser opts;
	opts.setApplicationDescription("Test DeliveryWizard");
	opts.addHelpOption();
	AppContext *agentContext=new AppContext(opts, env, "testDeliveryWizard", this);
	QVERIFY(nullptr!=agentContext);

	Q_INIT_RESOURCE(icons);
	Q_INIT_RESOURCE(images);
	Q_INIT_RESOURCE(qfi);
	Q_INIT_RESOURCE(3d);

	Node *testAgent=new Node(agentContext, DiscoveryRole::ROLE_AGENT, DiscoveryType::TYPE_AGENT, this);
	QVERIFY(nullptr!=testAgent);
	AgentDeliveryWizardTest *delWiz=new AgentDeliveryWizardTest();
	QVERIFY(nullptr!=delWiz);
	delWiz->configure(testAgent);
	delWiz->show();
	const int delay=50;
	QLineEdit *lineEditName=delWiz->findChild<QLineEdit *>("lineEditName");
	QString agentName="rnoldg";
	testLineEdit(lineEditName, "Arnold1337!!#g", agentName, delay);
	QStackedWidget *stackedWidget=delWiz->findChild<QStackedWidget*>("stackedWidget");
	QVERIFY(nullptr!=stackedWidget);
	QPushButton *pushButtonOnward=delWiz->findChild<QPushButton *>("pushButtonOnward");
	QVERIFY(nullptr!=pushButtonOnward);
	QVERIFY(pushButtonOnward->isEnabled());
	QVERIFY(pushButtonOnward->isVisible());
	QVERIFY("pageDelivery"==stackedWidget->currentWidget()->objectName());
	QSignalSpy spyStackedWidget(stackedWidget , SIGNAL(currentChanged(int)));
	//QSignalSpy spyPushButtonOnward(pushButtonOnward , SIGNAL(clicked(bool)));
	QTest::mouseClick(pushButtonOnward, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(), delay);
	//spyPushButtonOnward.wait();
	//QVERIFY(spyPushButtonOnward.count()>0)	;
	//qDebug()<<"onward button pushed";
	spyStackedWidget.wait(AgentDeliveryWizard::MINIMUM_BIRTH_TIME*3);
	QVERIFY(spyStackedWidget.count()==1)	;
	qDebug()<< "stacked widget swapped to" <<stackedWidget->currentWidget()->objectName();
	QVERIFY("pageBirthInProgress"==stackedWidget->currentWidget()->objectName());
	spyStackedWidget.wait(AgentDeliveryWizard::MINIMUM_BIRTH_TIME*3);
	QVERIFY(spyStackedWidget.count()==2)	;
	qDebug()<< "stacked widget swapped to" <<stackedWidget->currentWidget()->objectName();
	QVERIFY("pageBirthDone"==stackedWidget->currentWidget()->objectName());
	delete delWiz;
	delWiz=nullptr;
}



QTEST_MAIN(TestDeliveryWizard)

