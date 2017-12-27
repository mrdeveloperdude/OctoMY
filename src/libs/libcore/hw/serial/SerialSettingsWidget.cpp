#include "SerialSettingsWidget.hpp"

#include "ui_SerialSettingsWidget.h"

#include "utility/Standard.hpp"

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
	ui->setupUi(this);

	mIntValidator = OC_NEW QIntValidator(0, 4000000, this);

	ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

	connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)), this, SLOT(showPortInfo(int)));
	connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomBaudRatePolicy(int)));
	connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomDevicePathPolicy(int)));

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
	delete ui;
	ui=nullptr;
	delete mIntValidator;
	mIntValidator=nullptr;
}


void SerialSettingsWidget::configure(bool manualSaveMode, SerialSettings defaults)
{
	mManualSaveMode=manualSaveMode;
	ui->pushButtonApply->setVisible(manualSaveMode);
	mDefaultSettings=defaults;
}

SerialSettings SerialSettingsWidget::settings() const
{
	return mCurrentSettings;
}

void SerialSettingsWidget::showPortInfo(int idx)
{
	if (idx == -1) {
		return;
	}

	QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
	ui->descriptionLabel->setText(tr("%1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
	ui->manufacturerLabel->setText(tr("%1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
	ui->serialNumberLabel->setText(tr("%1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
	ui->locationLabel->setText(tr("%1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
	ui->vidLabel->setText(tr("%1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
	ui->pidLabel->setText(tr("%1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SerialSettingsWidget::apply()
{
	updateSettings();
	emit settingsChanged();
}

void SerialSettingsWidget::checkCustomBaudRatePolicy(int idx)
{
	bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
	ui->baudRateBox->setEditable(isCustomBaudRate);
	if (isCustomBaudRate) {
		ui->baudRateBox->clearEditText();
		QLineEdit *edit = ui->baudRateBox->lineEdit();
		edit->setValidator(mIntValidator);
	}
}

void SerialSettingsWidget::checkCustomDevicePathPolicy(int idx)
{
	bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
	ui->serialPortInfoListBox->setEditable(isCustomPath);
	if (isCustomPath) {
		ui->serialPortInfoListBox->clearEditText();
	}
}

void SerialSettingsWidget::fillPortsParameters()
{

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
	if(!mManualSaveMode) {
		// qDebug()<<"AUTOSAVE!";
		apply();
	}
}

void SerialSettingsWidget::onSerialDevicesChanged()
{
	fillPortsInfo();
}

void SerialSettingsWidget::on_pushButtonApply_clicked()
{
	apply();
}

void SerialSettingsWidget::on_pushButtonSelectDefaults_clicked()
{
	setSettings(mDefaultSettings);
}

void SerialSettingsWidget::on_pushButtonAdvanced_toggled(bool checked)
{
	showAdvanced(checked);
}



void SerialSettingsWidget::showAdvanced(bool show)
{
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

void SerialSettingsWidget::on_serialPortInfoListBox_currentIndexChanged(int index)
{
	registerChange();
}

void SerialSettingsWidget::on_baudRateBox_currentIndexChanged(int index)
{
	registerChange();
}

void SerialSettingsWidget::on_dataBitsBox_currentIndexChanged(int index)
{
	registerChange();
}

void SerialSettingsWidget::on_parityBox_currentIndexChanged(int index)
{
	registerChange();
}

void SerialSettingsWidget::on_stopBitsBox_currentIndexChanged(int index)
{
	registerChange();
}

void SerialSettingsWidget::on_flowControlBox_currentIndexChanged(int index)
{
	registerChange();
}

void SerialSettingsWidget::on_serialPortInfoListBox_editTextChanged(const QString &arg1)
{
	registerChange();
}
