#ifndef SERIALSETTINGSDIALOG_H
#define SERIALSETTINGSDIALOG_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>

#include "SerialSettings.hpp"

#include "SerialList.hpp"




namespace Ui
{
class SerialSettingsWidget;
}

class QIntValidator;


class SerialSettingsWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::SerialSettingsWidget *ui;
	SerialSettings currentSettings;
	QIntValidator *intValidator;
	QList<QSerialPortInfo> availablePorts;
	SerialList sl;

public:

	explicit SerialSettingsWidget(QWidget *parent = 0);
	~SerialSettingsWidget();

	SerialSettings settings() const;

public Q_SLOTS:


private slots:
	void onSerialDevicesChanged();
	void showPortInfo(int idx);
	void apply();
	void checkCustomBaudRatePolicy(int idx);
	void checkCustomDevicePathPolicy(int idx);

signals:

	void settingsChanged();

private:
	void fillPortsParameters();
	void fillPortsInfo();
	void updateSettings();


};

#endif // SERIALSETTINGSDIALOG_H



