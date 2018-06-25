#include "TestMapEditor.hpp"

#include "widgets/MapEditor.hpp"

#include <QComboBox>
#include <QNetworkReply>
#include <QSignalSpy>
/*
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
*/


// From: https://stackoverflow.com/questions/46656395/how-can-i-wait-for-a-qwidget-to-close-using-qtestlib-qt5
static bool waitForWidgetToClose(QWidget *widget, qint64 ms=-1)
{
	if(nullptr!=widget) {
		widget->setAttribute(Qt::WA_DeleteOnClose);
		widget->show();
		QSignalSpy spy(widget , SIGNAL(destroyed(QObject*)));
		if(ms<0) {
			// -1 means wait "forever"
			ms=1000000000;
		}
		if(!spy.wait(ms)) {
			return false;
		}
		return true;
	}
	return false;
}

void TestMapEditor::testNetworkCache()
{
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
	diskCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+"/map.cache");
	manager->setCache(diskCache);


	//qRegisterMetaType<QNetworkReply>("QNetworkReply");
	//qRegisterMetaType<QNetworkReply *>("QNetworkReply *");
	connect(manager, &QNetworkAccessManager::finished, [](QNetworkReply *reply) {
		qDebug() << "Object "<< reply->url()<< " loaded " << (reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool()?"[CACHED]":"[RAW]");
	});
	QSignalSpy spy(manager, &QNetworkAccessManager::finished);

	const QUrl url(QString("http://sau.no/"));
	// Do a normal request (preferred from network, as this is the default)
	QNetworkRequest request(url);
	request.setAttribute( QNetworkRequest::CacheSaveControlAttribute, true );
	manager->get(request);

	const int num=10;
	for(int i=0; i<num; ++i) {
		// Do a request preferred from cache
		QNetworkRequest request2(url);
		request2.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
		request2.setAttribute( QNetworkRequest::CacheSaveControlAttribute, true );
		manager->get(request2);
	}

	while(spy.count()<1+num) {
		spy.wait(1000);
	}
}

void TestMapEditor::test()
{
	MapEditor *ui=OC_NEW MapEditor();
	QVERIFY(nullptr!=ui);
	if(nullptr!=ui) {
		ui->show();

		QComboBox *comboBoxMapType=ui->findChild<QComboBox *>("comboBoxMapType");
		QVERIFY(nullptr != comboBoxMapType);
		comboBoxMapType->setCurrentIndex(0);
		QTest::waitForEvents();
		comboBoxMapType->setCurrentIndex(1);
		QTest::waitForEvents();
		waitForWidgetToClose(ui);

		/*
		const int delay=50;
		QLineEdit *lineEditName=ui->findChild<QLineEdit *>("lineEditName");
		QString agentName="arnold";
		testLineEdit(lineEditName, "AarnXol1337!!#d", agentName, delay);
		QStackedWidget *stackedWidget=ui->findChild<QStackedWidget*>("stackedWidget");
		QVERIFY(nullptr!=stackedWidget);
		QPushButton *pushButtonOnward=ui->findChild<QPushButton *>("pushButtonOnward");
		QVERIFY(nullptr!=pushButtonOnward);
		QVERIFY(pushButtonOnward->isEnabled());
		QVERIFY(pushButtonOnward->isVisible());
		QCOMPARE(stackedWidget->currentWidget()->objectName(), QString("pageDelivery"));
		QSignalSpy spyStackedWidget(stackedWidget , SIGNAL(currentChanged(int)));
		QCOMPARE(spyStackedWidget.count(), 0);
		//QSignalSpy spyPushButtonOnward(pushButtonOnward , SIGNAL(clicked(bool)));
		QTest::mouseClick(pushButtonOnward, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(), delay);
		QCOMPARE(stackedWidget->currentWidget()->objectName(), QString("pageBirthInProgress"));
		QCOMPARE(spyStackedWidget.count(), 1);
		if(!spyStackedWidget.wait(AgentDeliveryWizard::MINIMUM_BIRTH_TIME*20)) {
			qWarning()<<"WAIT #1 FAILED";
		}
		QCOMPARE(spyStackedWidget.count(), 2);
		QCOMPARE(stackedWidget->currentWidget()->objectName(), QString("pageDone"));
		*/
		ui->deleteLater();
		ui=nullptr;
	}
}


OC_TEST_MAIN(test, TestMapEditor)




