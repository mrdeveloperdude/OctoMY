#include "AudioSourceSelector.hpp"
#include "ui_AudioSourceSelector.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "audio/utils/AudioUtils.hpp"
#include "audio/AudioInputPreview.hpp"
#include "audio/AudioOutputPreview.hpp"

#include "utility/ui/Ui.hpp"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QWidget>
#include <QPushButton>
#include <QDebug>

AudioSourceSelector::AudioSourceSelector(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AudioSourceSelector)
{
	OC_METHODGATE();
	ui->setupUi(this);
	updateSources(true, true);
}

AudioSourceSelector::~AudioSourceSelector()
{
	OC_METHODGATE();
	delete ui;
}

void AudioSourceSelector::clearSources(){
	OC_METHODGATE();
	utility::ui::clearLayout(ui->verticalLayout);
}

void AudioSourceSelector::addSource(QAudioDevice device){
	auto scrollAreaWidgetContents = ui->scrollAreaWidgetContents;
	
	auto widget = new QWidget(scrollAreaWidgetContents);
	widget->setObjectName("widget");
	ui->verticalLayout->addWidget(widget);
	
	auto horizontalLayout= new QHBoxLayout(widget);
	horizontalLayout->setObjectName("AudioSourceLayout");
	
	QWidget *payload{nullptr};
	
	if(device.mode() == QAudioDevice::Input){
		qDebug()<<"INSERTING INPUT DEVICE"<<device.description();
		
		auto widgetSource = new AudioInputPreview(scrollAreaWidgetContents);
		widgetSource->setAudioInput(device.description());
		widgetSource->setObjectName("widgetSource");
		payload=widgetSource;
	}
	else{
		qDebug()<<"INSERTING OUTPUT DEVICE"<<device.description();
		auto widgetOutput = new AudioOutputPreview(scrollAreaWidgetContents);
		widgetOutput->setAudioOutput(device.description());
		widgetOutput->setObjectName("widgetOutput");
		payload=widgetOutput;
	}
	
	QSizePolicy sizePolicyPreview(QSizePolicy::Expanding, QSizePolicy::Minimum);
	sizePolicyPreview.setHorizontalStretch(0);
	sizePolicyPreview.setVerticalStretch(0);
	sizePolicyPreview.setHeightForWidth(payload->sizePolicy().hasHeightForWidth());
	payload->setSizePolicy(sizePolicyPreview);
	horizontalLayout->addWidget(payload);
	
	auto pushButtonSelect = new QPushButton(scrollAreaWidgetContents);
	pushButtonSelect->setObjectName("pushButtonSelect");
	QSizePolicy sizePolicyButton(QSizePolicy::Preferred, QSizePolicy::Minimum);
	sizePolicyButton.setHorizontalStretch(0);
	sizePolicyButton.setVerticalStretch(0);
	sizePolicyButton.setHeightForWidth(pushButtonSelect->sizePolicy().hasHeightForWidth());
	pushButtonSelect->setSizePolicy(sizePolicyButton);
	pushButtonSelect->setMinimumSize(QSize(30, 0));
	horizontalLayout->addWidget(pushButtonSelect);
	pushButtonSelect->setText(QCoreApplication::translate("AudioSourceSelector", "Select", nullptr));
	
	if(!connect(pushButtonSelect, &QPushButton::clicked, this, [this, device](){
				emit deviceSelected(device);
}, OC_CONTYPE_NON_UNIQUE)){
		qWarning()<<"Could not connect select button";
	}
}


void AudioSourceSelector::updateSources(bool showInputs, bool showOutputs){
	OC_METHODGATE();
	clearSources();
	QAudioFormat format;
	format.setSampleFormat(QAudioFormat::Int16);
	format.setSampleRate(44100);
	format.setChannelCount(1);
	if(showInputs){
		auto devices = availableAudioDevices(format, false);
		qDebug()<<"DEVICES:";
		for(auto &device:devices){
			qDebug()<<device.description()<< (device.isDefault()?" (DEFAULT)":"");
			addSource(device);
		}
	}
	if(showOutputs){
		auto devices = availableAudioDevices(format, true);
		qDebug()<<"DEVICES:";
		for(auto &device:devices){
			qDebug()<<device.description()<< (device.isDefault()?" (DEFAULT)":"");
			addSource(device);
		}
	}
	
	auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addItem(verticalSpacer);
}


void AudioSourceSelector::on_pushButtonCancel_clicked()
{
	emit deviceSelected(QAudioDevice());
}

