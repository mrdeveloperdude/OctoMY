#include "ActivityStack.hpp"

#include <QVBoxLayout>

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

ActivityStack::ActivityStack(QWidget *parent)
	: QWidget(parent)
	, mBottom(OC_NEW Activity(this))
	, mStackedWidget(OC_NEW QStackedWidget(this))
{
	OC_METHODGATE();
	mBottom->setObjectName("ActivityStackBottomPage");
	registerPage(mBottom);
	mStack.push_back(mBottom->objectName());
	QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(mStackedWidget->sizePolicy().hasHeightForWidth());
	mStackedWidget->setSizePolicy(sizePolicy);
	auto verticalLayout = new QVBoxLayout(this);
	verticalLayout->setContentsMargins(0, 0, 0, 0);
	verticalLayout->addWidget(mStackedWidget);
}

ActivityStack::~ActivityStack()
{
	OC_METHODGATE();
}


QString ActivityStack::pop(){
	if(mStack.size() > 1){
		auto back = mStack.back();
		mStack.pop_back();
		auto title = mStack.back();
		if(mPageMap.contains(title)){
			auto page = mPageMap[title];
			mStackedWidget->setCurrentWidget(page.widget);
			return back;
		}
		else{
			// We failed, put it back
			mStack.push_back(back);
		}
	}
	else{
		qWarning() << "stack empty trying to pop";
	}
	return "";
}

bool ActivityStack::push(const QString &title){
	if(mPageMap.contains(title)){
		auto page = mPageMap[title];
		if(nullptr != page.widget){
			mStack.push_back(title);
			mStackedWidget->setCurrentWidget(page.widget);
			return true;
		}
		else{
			qWarning() << "page.widget was null";
		}
	}
	else{
		qWarning() << "no page named " << title << " registered";
	}
	return false;
}



QString ActivityStack::swap(const QString &title){
	auto back = pop();
	if("" != back){
		if(push(title)){
			return back;
		}
	}
	return "";
}


void ActivityStack::registerPage(Activity *page, bool headerEnabled, bool backEnabled, bool menuEnabled){
	OC_METHODGATE();
	auto title = page->objectName();
	mPageMap[title] = StackPage(page, headerEnabled, backEnabled, menuEnabled);
	mStackedWidget->addWidget(page);
}
