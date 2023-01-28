#include "AudioOutputPreview.hpp"
#include "ui_AudioOutputPreview.h"

#include "audio/utils/AudioUtils.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include <QAudioDevice>
#include <QTimer>


AudioOutputPreview::AudioOutputPreview(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AudioOutputPreview)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(!mClickCon){
		mClickCon = connect(ui->pushButtonTest, &QPushButton::clicked, this, &AudioOutputPreview::onTestClicked, OC_CONTYPE);
		if(!mClickCon){
			qWarning()<<"Could not connect test button";
		}
	}
}


AudioOutputPreview::~AudioOutputPreview()
{
	OC_METHODGATE();
	delete ui;
}


void AudioOutputPreview::setAudioOutput(QString audioDeviceName){
	OC_METHODGATE();
	mAudioDeviceName=audioDeviceName;
	auto device = audioDeviceForName(mAudioDeviceName, true);
	ui->labelDescription->setText(device.description());
	ui->labelMinSampleRate->setText(QString::number(device.minimumSampleRate()));
	ui->labelMaxSampleRate->setText(QString::number(device.maximumSampleRate()));
	ui->labelSampleFormat->setText(sampleFormatsToString(device.supportedSampleFormats()));
	ui->labelChannelConfiguration->setText(channelConfigurationToString(device.channelConfiguration()));
}


void AudioOutputPreview::onTestClicked(){
	OC_METHODGATE();
	qWarning()<<"Test clicked";
	auto device = audioDeviceForName(mAudioDeviceName, true);
	/*
	ui->pushButtonTest->setDisabled(true);
	QTimer::singleShot(3000, [=](){
		ui->pushButtonTest->setDisabled(false);
	});
	*/
}
