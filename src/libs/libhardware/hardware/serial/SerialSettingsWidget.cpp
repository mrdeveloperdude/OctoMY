#include "SerialSettingsWidget.hpp"

#include "ui_SerialSettingsWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QDebug>


static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SerialSettingsWidget::SerialSettingsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::SerialSettingsWidget)
	, mManualSaveMode(false)
{
	OC_METHODGATE();
	ui->setupUi(this);

	mIntValidator = OC_NEW QIntValidator(0, 4000000, this);
	ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);
	if(!connect(&mSerialList,SIGNAL(serialDevicesChanged()),this,SLOT(onSerialDevicesChanged()))) {
		qWarning()<<"ERROR: could not connect";
	}
	fillPortsParameters();
	fillPortsInfo();
	updateSettings();
	showAdvanced(false);
}


SerialSettingsWidget::~SerialSettingsWidget()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
	delete mIntValidator;
	mIntValidator = nullptr;
}


void SerialSettingsWidget::configure(bool manualSaveMode, SerialSettings defaults)
{
	OC_METHODGATE();
	mManualSaveMode = manualSaveMode;
	ui->pushButtonApply->setVisible(manualSaveMode);
	mDefaultSettings = defaults;
	setSettings(mDefaultSettings);
}


SerialSettings SerialSettingsWidget::settings() const
{
	OC_METHODGATE();
	return mCurrentSettings;
}


void SerialSettingsWidget::showPortInfo(int index)
{
	OC_METHODGATE();
	if (index == -1) {
		return;
	}
	QStringList list = ui->serialPortInfoListBox->itemData(index).toStringList();
	ui->descriptionLabel->setText(tr("%1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
	ui->manufacturerLabel->setText(tr("%1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
	ui->serialNumberLabel->setText(tr("%1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
	ui->locationLabel->setText(tr("%1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
	ui->vidLabel->setText(tr("%1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
	ui->pidLabel->setText(tr("%1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}


void SerialSettingsWidget::checkCustomBaudRatePolicy(int index)
{
	OC_METHODGATE();
	bool isCustomBaudRate = !ui->baudRateBox->itemData(index).isValid();
	ui->baudRateBox->setEditable(isCustomBaudRate);
	if (isCustomBaudRate) {
		ui->baudRateBox->clearEditText();
		QLineEdit *edit = ui->baudRateBox->lineEdit();
		edit->setValidator(mIntValidator);
	}
}


void SerialSettingsWidget::checkCustomDevicePath(int index)
{
	OC_METHODGATE();
	bool isCustomPath = !ui->serialPortInfoListBox->itemData(index).isValid();
	ui->serialPortInfoListBox->setEditable(isCustomPath);
	if (isCustomPath) {
		ui->serialPortInfoListBox->clearEditText();
	}
}


void SerialSettingsWidget::baudRateChanged(int index)
{
	checkCustomBaudRatePolicy(index);
	registerChange();
}


void SerialSettingsWidget::serialPortChanged(int index)
{
	showPortInfo(index);
	checkCustomDevicePath(index);
	registerChange();
}


void SerialSettingsWidget::applySettings()
{
	OC_METHODGATE();
	updateSettings();
	emit settingsChanged();
}


void SerialSettingsWidget::fillPortsParameters()
{
	OC_METHODGATE();

#define addBaudRate(A) ui->baudRateBox->addItem(baudRateToString(QSerialPort::A), QSerialPort::A)
	addBaudRate(Baud9600);
	addBaudRate(Baud19200);
	addBaudRate(Baud38400);
	addBaudRate(Baud115200);
	ui->baudRateBox->addItem(tr("Custom"));
#undef addBaudRate

#define addDataBits(A) ui->dataBitsBox->addItem(dataBitsToString(QSerialPort::A), QSerialPort::A)
	addDataBits(Data5);
	addDataBits(Data6);
	addDataBits(Data7);
	addDataBits(Data8);
#undef addDataBits
	ui->dataBitsBox->setCurrentIndex(3);

#define addParity(A) ui->parityBox->addItem(parityToString(QSerialPort::A), QSerialPort::A)
	addParity(NoParity);
	addParity(EvenParity);
	addParity(OddParity);
	addParity(MarkParity);
	addParity(SpaceParity);
#undef addParity

#define addStopBits(A) ui->stopBitsBox->addItem(stopBitsToString(QSerialPort::A), QSerialPort::A)
	addStopBits(OneStop);
	addStopBits(TwoStop);
#undef addStopBits

#define addFlowControl(A) ui->flowControlBox->addItem(flowControlToString(QSerialPort::A), QSerialPort::A)
	addFlowControl(NoFlowControl);
	addFlowControl(HardwareControl);
	addFlowControl(SoftwareControl);
#undef addFlowControl

}


void SerialSettingsWidget::fillPortsInfo()
{
	OC_METHODGATE();
	ui->serialPortInfoListBox->clear();
	QString description;
	QString manufacturer;
	QString serialNumber;
	//qDebug()<<"SERIAL DEVICES FOUND:";
	QList<QSerialPortInfo> availablePorts=QSerialPortInfo::availablePorts();
	for(QList<QSerialPortInfo>::iterator it=availablePorts.begin(),e=availablePorts.end(); it!=e; ++it) {
		const QSerialPortInfo info=*it;
		QStringList list;
		description = info.description();
		manufacturer = info.manufacturer();
		serialNumber = info.serialNumber();
		//qDebug()<<" + "<<info.portName();
		list << info.portName()
			 << (!description.isEmpty() ? description : blankString)
			 << (!manufacturer.isEmpty() ? manufacturer : blankString)
			 << (!serialNumber.isEmpty() ? serialNumber : blankString)
			 << info.systemLocation()
			 << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
			 << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

		ui->serialPortInfoListBox->addItem(list.first(), list);
	}
	ui->serialPortInfoListBox->addItem(tr("Custom"));
}


void SerialSettingsWidget::updateSettings()
{
	OC_METHODGATE();
	mCurrentSettings.name = ui->serialPortInfoListBox->currentText();
	if (ui->baudRateBox->currentIndex() == 4) {
		mCurrentSettings.setIntBaudRate(ui->baudRateBox->currentText().toInt());
	} else {
		mCurrentSettings.baudRate = static_cast<QSerialPort::BaudRate>( ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
	}
	mCurrentSettings.dataBits = static_cast<QSerialPort::DataBits>( ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
	mCurrentSettings.parity = static_cast<QSerialPort::Parity>( ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
	mCurrentSettings.stopBits = static_cast<QSerialPort::StopBits>( ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
	mCurrentSettings.flowControl = static_cast<QSerialPort::FlowControl>( ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
}


void SerialSettingsWidget::setSettings(SerialSettings &settings)
{
	OC_METHODGATE();
	qDebug()<< "SERIAL SETTINGS WIDGET RECEIVED SETTINGS: "<<serialSettingsToString(settings);
	ui->serialPortInfoListBox->setCurrentText(settings.name);
	ui->baudRateBox->setCurrentText(QString::number(settings.baudRate));
	ui->dataBitsBox->setCurrentIndex(settings.dataBits-5);
	ui->parityBox->setCurrentIndex(settings.parity);
	ui->stopBitsBox->setCurrentIndex(settings.stopBits-1);
	ui->flowControlBox->setCurrentIndex(settings.flowControl);
	updateSettings();
}


void SerialSettingsWidget::registerChange()
{
	OC_METHODGATE();
	if(!mManualSaveMode) {
		// qDebug()<<"AUTOSAVE!";
		applySettings();
	}
}


void SerialSettingsWidget::onSerialDevicesChanged()
{
	OC_METHODGATE();
	fillPortsInfo();
}


void SerialSettingsWidget::selectDefaults()
{
	OC_METHODGATE();
	setSettings(mDefaultSettings);
}


void SerialSettingsWidget::toggleAdvanced(bool checked)
{
	OC_METHODGATE();
	showAdvanced(checked);
}


void SerialSettingsWidget::showAdvanced(bool show)
{
	OC_METHODGATE();
	ui->baudRateBox->setVisible(show);
	ui->baudRateLabel->setVisible(show);
	ui->dataBitsBox->setVisible(show);
	ui->dataBitsLabel->setVisible(show);
	ui->descriptionLabel->setVisible(show);
	ui->flowControlBox->setVisible(show);
	ui->flowControlLabel->setVisible(show);
	ui->labelTagDescription->setVisible(show);
	ui->labelTagProductID->setVisible(show);
	ui->labelTagSerialNumber->setVisible(show);
	ui->labelTagVendorID->setVisible(show);
	ui->parityBox->setVisible(show);
	ui->parityLabel->setVisible(show);
	ui->pidLabel->setVisible(show);
	ui->serialNumberLabel->setVisible(show);
	ui->stopBitsBox->setVisible(show);
	ui->stopBitsLabel->setVisible(show);
	ui->vidLabel->setVisible(show);
	if(show) {
		ui->pushButtonAdvanced->setText("Hide Advanced");
	} else {
		ui->pushButtonAdvanced->setText("Show Advanced");
	}
}


void SerialSettingsWidget::dataBitsChanged(int index)
{
	OC_METHODGATE();
	Q_UNUSED(index);
	registerChange();
}


void SerialSettingsWidget::parityChanged(int index)
{
	OC_METHODGATE();
	Q_UNUSED(index);
	registerChange();
}


void SerialSettingsWidget::stopBitsChanged(int index)
{
	OC_METHODGATE();
	Q_UNUSED(index);
	registerChange();
}


void SerialSettingsWidget::flowControlChanged(int index)
{
	OC_METHODGATE();
	Q_UNUSED(index);
	registerChange();
}


void SerialSettingsWidget::serialPortTextChanged(const QString &arg1)
{
	OC_METHODGATE();
	Q_UNUSED(arg1);
	registerChange();
}
