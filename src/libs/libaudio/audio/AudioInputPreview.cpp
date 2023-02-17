#include "AudioInputPreview.hpp"

#include "ui_AudioInputPreview.h"

#include "audio/utils/AudioUtils.hpp"
#include "uptime/MethodGate.hpp"

#include <QAudioDevice>

AudioInputPreview::AudioInputPreview(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AudioInputPreview)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


AudioInputPreview::~AudioInputPreview()
{
	OC_METHODGATE();
	delete ui;
}


void AudioInputPreview::setAudioInput(QString inputName){
	OC_METHODGATE();
	ui->audioPreview->setInput(inputName);
	auto device = audioDeviceForName(inputName, false);
	ui->labelDescription->setText(device.description());
	ui->labelMinSampleRate->setText(QString::number(device.minimumSampleRate()));
	ui->labelMaxSampleRate->setText(QString::number(device.maximumSampleRate()));
	ui->labelSampleFormat->setText(sampleFormatsToString(device.supportedSampleFormats()));
	ui->labelChannelConfiguration->setText(channelConfigurationToString(device.channelConfiguration()));
}
