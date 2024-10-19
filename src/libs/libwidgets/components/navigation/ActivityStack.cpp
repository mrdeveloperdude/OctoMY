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


QString ActivityStack::pop(const QStringList returnArguments)
{
	OC_METHODGATE();
	if(mStack.size() > 1){
		auto back = mStack.back();
		mStack.pop_back();
		auto title = mStack.back();
		if(mPageMap.contains(title)){
			auto page = mPageMap[title];
			auto lastPage = mStackedWidget->currentWidget();
			mStackedWidget->setCurrentWidget(page.widget);
			page.widget->popImpl_(lastPage->objectName(), returnArguments);
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


bool ActivityStack::push(const QString &title, const QStringList arguments)
{
	OC_METHODGATE();
	if(mPageMap.contains(title)){
		auto page = mPageMap[title];
		if(nullptr != page.widget){
			mStack.push_back(title);
			mStackedWidget->setCurrentWidget(page.widget);
			page.widget->pushImpl_(arguments);
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


QString ActivityStack::swap(const QString &title, const QStringList returnArguments, const QStringList arguments)
{
	OC_METHODGATE();
	auto back = pop(returnArguments);
	if("" != back){
		if(push(title, arguments)){
			return back;
		}
	}
	return "";
}


void ActivityStack::registerPage(Activity *page, bool headerEnabled, bool backEnabled, bool menuEnabled){
	OC_METHODGATE();
	auto title = page->objectName();
	mPageMap[title] = StackPage(page, headerEnabled, backEnabled, menuEnabled);
	page->setActivityStack(this);
	mStackedWidget->addWidget(page);
}
