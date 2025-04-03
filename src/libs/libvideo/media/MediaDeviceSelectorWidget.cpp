#include "MediaDeviceSelectorWidget.hpp"
#include "ui_MediaDeviceSelectorWidget.h"

#include "media/MediaDeviceManager.hpp"
#include "media/MediaDeviceModel.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"

#include <QSharedPointer>

MediaDeviceSelectorWidget::MediaDeviceSelectorWidget(
	QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::MediaDeviceSelectorWidget)
	, mManager{QSharedPointer <MediaDeviceManager>::create()}
	, mModel{QSharedPointer <MediaDeviceModel>::create(mManager)}
{
	OC_METHODGATE();
	ui->setupUi(this);
}

MediaDeviceSelectorWidget::~MediaDeviceSelectorWidget()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
}



void MediaDeviceSelectorWidget::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	mNode = node;
	ui->widgeMediaDevices->configure(node->settings(), "alla", "balla");
	ui->widgeMediaDevices->setModel(mModel.data());
}


void MediaDeviceSelectorWidget::togglePreview(bool preview){
	OC_METHODGATE();
	mModel->setPreviewActive(preview);
}
