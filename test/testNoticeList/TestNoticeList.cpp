#include "TestNoticeList.hpp"

#include "components/NoticeList.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"


#include <QPushButton>
#include <QVBoxLayout>
/*
void TestNoticeList::test()
{
	Q_INIT_RESOURCE(icons);
	auto w = OC_NEW NoticeList();
	w->show();
	test::utility::waitForUIEnd(w);
}

*/

void TestNoticeList::test()
{
	Q_INIT_RESOURCE(icons);
	
	// Create a main widget to hold the button and NoticeList
	QWidget mainWidget;
	mainWidget.setWindowTitle("Test NoticeList with Add Button");
	
	// Create a vertical layout for the main widget
	QVBoxLayout *layout = new QVBoxLayout(&mainWidget);
	
	// Create the QPushButton to add notices
	QPushButton *addButton = new QPushButton("Add Random Notice", &mainWidget);
	layout->addWidget(addButton);
	
	// Create the NoticeList widget
	NoticeList *noticeList = OC_NEW NoticeList(&mainWidget);
	layout->addWidget(noticeList);
	int i=1;
	// Connect the button's clicked signal to a lambda that adds a random notice
	QObject::connect(addButton, &QPushButton::clicked, [&]() {
		noticeList->appendNotice(QString("Notice %1").arg(i));
		i++;
	});
	
	mainWidget.show();
	
	// Wait for UI interaction
	test::utility::waitForUIEnd(&mainWidget);
}

OC_TEST_MAIN(test, TestNoticeList)


