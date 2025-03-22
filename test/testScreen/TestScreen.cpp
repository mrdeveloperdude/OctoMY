#include "TestScreen.hpp"

#include "screen/ScreenManager.hpp"
#include "screen/ScreenSelector.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"

#include <QPushButton>


void TestScreen::testManager()
{
	Q_INIT_RESOURCE(icons);
	auto sm = ScreenManager::instance();
	sm->selectScreen();
	auto qb=new QPushButton();
	qb->setText("Quit");
	qb->show();
	sm->selectScreen();

	test::utility::waitForUIEnd(qb);
}

void TestScreen::testSelector()
{
	Q_INIT_RESOURCE(icons);
	auto ss = OC_NEW ScreenSelector();
	ss->setScreen(QGuiApplication::primaryScreen());
	ss->show();

	test::utility::waitForUIEnd(ss);
}
OC_TEST_MAIN(test, TestScreen)

