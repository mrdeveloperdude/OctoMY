#include "SerialSettingsWidget.hpp"

#include "ui_SerialSettingsWidget.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QDebug>


static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SerialSettingsWidget::SerialSettingsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SerialSettingsWidget)
{
	ui->setupUi(this);

	mIntValidator = new QIntValidator(0, 4000000, this);

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
}

SerialSettingsWidget::~SerialSettingsWidget()
{
	delete ui;
}


void SerialSettingsWidget::configure(bool showApplyButton, SerialSettings defaults)
{
	ui->pushButtonApply->setVisible(showApplyButton);
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
	ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
	ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
	ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
	ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
	ui->baudRateBox->addItem(tr("Custom"));

	ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
	ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
	ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
	ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
	ui->dataBitsBox->setCurrentIndex(3);

	ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
	ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
	ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
	ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
	ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

	ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);

	ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

	ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
	ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
	ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
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
		mCurrentSettings.baudRate = ui->baudRateBox->currentText().toInt();
	} else {
		mCurrentSettings.baudRate = static_cast<QSerialPort::BaudRate>( ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
	}
	mCurrentSettings.stringBaudRate = QString::number(mCurrentSettings.baudRate);

	mCurrentSettings.dataBits = static_cast<QSerialPort::DataBits>( ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
	mCurrentSettings.stringDataBits = ui->dataBitsBox->currentText();

	mCurrentSettings.parity = static_cast<QSerialPort::Parity>( ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
	mCurrentSettings.stringParity = ui->parityBox->currentText();

	mCurrentSettings.stopBits = static_cast<QSerialPort::StopBits>( ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
	mCurrentSettings.stringStopBits = ui->stopBitsBox->currentText();

	mCurrentSettings.flowControl = static_cast<QSerialPort::FlowControl>( ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
	mCurrentSettings.stringFlowControl = ui->flowControlBox->currentText();

}



void SerialSettingsWidget::setSettings(SerialSettings &settings)
{
	ui->serialPortInfoListBox->setCurrentText(settings.name);
	ui->baudRateBox->setCurrentText(QString::number(settings.baudRate));
	ui->dataBitsBox->setCurrentIndex(settings.dataBits-5);
	ui->parityBox->setCurrentIndex(settings.parity);
	ui->stopBitsBox->setCurrentIndex(settings.stopBits-1);
	ui->flowControlBox->setCurrentIndex(settings.flowControl);
	updateSettings();
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
