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
	SerialSettings mDefaultSettings;
	SerialSettings mCurrentSettings;
	QIntValidator *mIntValidator;
	QList<QSerialPortInfo> mAvailablePorts;
	SerialList mSerialList;

public:

	explicit SerialSettingsWidget(QWidget *parent = 0);
	~SerialSettingsWidget();

	void configure(bool showApplyButton=true, SerialSettings defaults=SerialSettings());
	SerialSettings settings() const;
	void setSettings(SerialSettings &settings);

public Q_SLOTS:


public slots:
	void onSerialDevicesChanged();
	void showPortInfo(int idx);
	void apply();
	void checkCustomBaudRatePolicy(int idx);
	void checkCustomDevicePathPolicy(int idx);

	void on_pushButtonApply_clicked();

signals:

	void settingsChanged();

public slots:
	void on_pushButtonSelectDefaults_clicked();

private:
	void fillPortsParameters();
	void fillPortsInfo();
	void updateSettings();


};

#endif // SERIALSETTINGSDIALOG_H
