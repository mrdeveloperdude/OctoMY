#include "TestMenuActivity.hpp"

#include "activities/MessageActivity.hpp"
#include "components/navigation/ActivityStack.hpp"
#include "navigation/MenuActivity.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"
#include "uptime/SharedPointerWrapper.hpp"


QSharedPointer<Activity> dummyMessage(){
	auto activity = QSharedPointer<MessageActivity>::create();
	return activity;
}

QSharedPointer<Activity> dummyMenu(){
	auto menu = QSharedPointer<MenuActivity>::create();
	for(int group=0;group<3;++group){
		auto groupName = QString("Group %1").arg(group);
		menu->addGroup(groupName);
		for(int button=0;button<30;++button){
			menu->addButton(QString("Button %1").arg(button), ":/icons/app/window/development.svg", "Some description", [](){});
		}
	}
	menu->addSpacer();
	for(int button=0;button<3;++button){
		menu->addButton(QString("Bottom Button %1").arg(button), ":/icons/app/window/development.svg", "Some description", [](){});
	}
	return menu;
}

void TestMenuActivity::test()
{
	Q_INIT_RESOURCE(icons);
	auto stack{OC_NEW ActivityStack()};
	stack->registerActivity(dummyMenu());
	stack->registerActivity(dummyMessage());
	stack->show();
	stack->push("MenuActivity", QStringList()<<"Title Text"<<"boDy text");
	stack->push("MessageActivity", QStringList()<<"Title Text"<<"boDy text");
	test::utility::waitForUIEnd(stack);
}


OC_TEST_MAIN(test, TestMenuActivity)

