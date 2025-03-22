#include "TestFlowLayout.hpp"

#include "layout/FlowLayout.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"

#include <QPushButton>


void TestFlowLayout::test()
{
	Q_INIT_RESOURCE(icons);
	auto w = OC_NEW QWidget();
	w->resize(800, 600);
	auto layout = OC_NEW FlowLayout(w);
	layout->setObjectName("flowLayout");
	
	for(int i=0;i<100;++i){
		auto button = OC_NEW QPushButton();
		button->setText("Press Me!");
		layout->addWidget(button);
	}
	w->show();
	test::utility::waitForUIEnd(w);
}


OC_TEST_MAIN(test, TestFlowLayout)

