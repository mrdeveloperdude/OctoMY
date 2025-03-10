#include "TestPingWidget.hpp"

#include "discovery/ping/PingWidget.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"


#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>


QWidget* createTestWidget(QWidget *parent = nullptr)
{
	auto *container = OC_NEW QWidget(parent);
	auto layout = OC_NEW QHBoxLayout(container);
	auto pingWidget = OC_NEW PingWidget(container);
	pingWidget->setFixedSize(200, 200);
	auto pingButton = OC_NEW QPushButton("Ping", container);
	auto pongButton = OC_NEW QPushButton("Pong", container);
	layout->addWidget(pingButton);
	layout->addWidget(pingWidget);
	layout->addWidget(pongButton);
	QObject::connect(pingButton, &QPushButton::clicked, pingWidget, [=](){
		pingWidget->ping(0xFF66FF22, true);
	});
	QObject::connect(pongButton, &QPushButton::clicked, pingWidget, [=](){
		pingWidget->ping(0xFF2266FF, false);
	});
	return container;
}


void TestPingWidget::test() {
	Q_INIT_RESOURCE(icons);
	Q_INIT_RESOURCE(images);
	auto testWidget = createTestWidget();
	testWidget ->show();

	// Wait for the UI to end (this blocks until the test window is closed)
	test::utility::waitForUIEnd(testWidget);
}



OC_TEST_MAIN(test, TestPingWidget)

