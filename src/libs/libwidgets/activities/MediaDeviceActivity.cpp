#include "MediaDeviceActivity.hpp"
#include "ui_MediaDeviceActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"

MediaDeviceActivity::MediaDeviceActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::MediaDeviceActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


MediaDeviceActivity::~MediaDeviceActivity()
{
	OC_METHODGATE();
	delete ui;
}

void MediaDeviceActivity::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	mNode = node;
	ui->widgetMediaDevices->configure(mNode);
}

void MediaDeviceActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void MediaDeviceActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}

/*
I am making a media device manager for my Qt6 app. The Archietcure is as follows:

I would like a central class that keeps track of all available media devices. Each device will have one or more streams attached to it.

For example when I plug in a USB webacm, QMediaDevices will report two separate events: the connection of the video source and the connection of the audio source.

I would like to wrap these events and automatically match them to a single device with two streams.

Second, I would like to wrap this universal "device manager" in a Qt style item model so that I can show it in lists, tables and trees.

I would like the preview of each device in views, when they are enabled to be the actual media, so a video source with no audio shows a strem of frames, an audio source shows live waveform and a webcam with both will show both.

When they are not enabled, the preview should show svg icons that represent each stream, so a video icon and 2 audio icons for a stereo webcam.

The text label for each item should be the device name and the body text should be the technical details about the devices such as codec, framerate, number of channels etc.

The model should have a filter where I can specify which devices I want to show. The filter should allow to select "only audio", "only video", "only audio+video", "any" (enum).

The model should have a setPreviewActive(bool) to turn on/off rendering of realtime preview for all the devices in display.

I already have this "multiview" class that allwos me to view the model in different ways:
*/
