#include "PageRegistry.hpp"

#include "uptime/MethodGate.hpp"


#include <QWidget>




PageRegistry::PageRegistry() {}


void PageRegistry::registerPage(QWidget *page, bool headerEnabled, bool backEnabled, bool menuEnabled){
	OC_METHODGATE();
	auto title = page->objectName();
	mPageMap[title] = RegisteredPage(page, headerEnabled, backEnabled, menuEnabled);
}


RegisteredPage PageRegistry::borrow(const QString &name){
	if(mPageMap.contains(name)){
		return mPageMap[name];
	}
	return RegisteredPage();
}
