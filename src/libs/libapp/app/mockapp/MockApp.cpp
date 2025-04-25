#include "MockApp.hpp"

#include "app/mockapp/MockAppWindow.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QObject>
#include <QLayout>
#include <QPushButton>
#include <QWidget>


//AppLauncher<MockApp> &launcher, NodeRole mockNodeRole, NodeType mockNodeType, QObject *parent
//AppLauncher<MockApp> &launcher, NodeRole mockNodeRole, NodeType mMockNodeType, QObject *parent = nullptr


MockApp::MockApp()
	: mWindow(OC_NEW MockAppWindow())
	, mNodeRole(ROLE_UNKNOWN)
	, mNodeType(TYPE_UNKNOWN) 
{
	OC_METHODGATE();
	qDebug()<<"MockApp::MockApp()";
}

MockApp::~MockApp()
{
	OC_METHODGATE();
	qDebug()<<"MockApp::~MockApp()";
	mWindow->deleteLater();
	mWindow=nullptr;
}



void MockApp::handleMock(QString event){
	OC_METHODGATE();
	qDebug()<<"MockApp::"+event+"()";
	emit mockEvent(event);
}


void MockApp::nodeConfigure(){
	OC_METHODGATE();
	handleMock("nodeConfigure");
}


void MockApp::nodeActivate(const bool on){
	OC_METHODGATE();
	handleMock("nodeActivate");
	emit nodeActivateChanged(on);
}


QSharedPointer<NodeWindow> MockApp::nodeWindow(){
	OC_METHODGATE();
	handleMock("nodeWindow");
	if(mWindow.isNull()) {
		QSharedPointer<MockApp> sp=qSharedPointerCast<MockApp>(sharedThis());
		if(!sp.isNull()) {
			mWindow=QSharedPointer<MockAppWindow>::create(nullptr);
			if(!mWindow.isNull()) {
				mWindow->nodeWindowConfigure(sp);
			} else {
				qWarning()<<"ERROR: Could not allocate MockAppWindow";
			}
		} else {
			qWarning()<<"ERROR: Shared pointer to this was null for mockapp";
		}
	}
	return mWindow;
}


NodeRole MockApp::nodeRole(){
	OC_METHODGATE();
	handleMock("nodeRole");
	return mNodeRole;
}


NodeType MockApp::nodeType(){
	OC_METHODGATE();
	handleMock("nodeType");
	return mNodeType;
}


QSharedPointer<Node> MockApp::sharedThis(){
	OC_METHODGATE();
	handleMock("sharedThis");
	return sharedFromThis();
}

