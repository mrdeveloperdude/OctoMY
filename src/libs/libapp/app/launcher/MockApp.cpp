#include "MockApp.hpp"

#include "uptime/New.hpp"

#include <QObject>
#include <QLayout>
#include <QPushButton>
#include <QWidget>


MockApp::MockApp(AppLauncher<MockApp> &launcher, QObject *parent)
	: QObject(parent)
	, mWindow(OC_NEW QWidget())
{
	OC_METHODGATE();
	qDebug()<<"MockApp::MockApp()";
	Q_UNUSED(launcher);
	Q_UNUSED(parent);
	QPushButton *button=OC_NEW QPushButton("QUIT", mWindow.data());
	QObject::connect(
	button, &QPushButton::clicked, this, [this]() {
		qDebug()<<"Quit clicked";
		emit appRequestClose();
	});

}

MockApp::~MockApp()
{
	OC_METHODGATE();
	qDebug()<<"MockApp::~MockApp()";
	mWindow->deleteLater();
	mWindow=nullptr;
}


void MockApp::appInit()
{
	OC_METHODGATE();
	qDebug()<<"MockApp::appInit()";
	emit appInitDone();
}

void MockApp::appDeInit()
{
	OC_METHODGATE();
	qDebug()<<"MockApp::appDeInit()";
	emit appDeInitDone();
}

QSharedPointer<QWidget> MockApp::appWindow()
{
	OC_METHODGATE();
	qDebug()<<"MockApp::appWindow()";

	return mWindow;
}


