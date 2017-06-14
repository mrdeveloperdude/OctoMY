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
	bool mManualSaveMode;

public:

	explicit SerialSettingsWidget(QWidget *parent = 0);
	~SerialSettingsWidget();

	void configure(bool manualSaveMode=true, SerialSettings defaults=SerialSettings());
	SerialSettings settings() const;
	void setSettings(SerialSettings &settings);

private:
	// If in auto save mode, triggers save. If not does nothing
	void registerChange();

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

private slots:
	void on_pushButtonAdvanced_toggled(bool checked);
	void on_serialPortInfoListBox_currentIndexChanged(int index);
	void on_baudRateBox_currentIndexChanged(int index);
	void on_dataBitsBox_currentIndexChanged(int index);
	void on_parityBox_currentIndexChanged(int index);
	void on_stopBitsBox_currentIndexChanged(int index);
	void on_flowControlBox_currentIndexChanged(int index);
	void on_serialPortInfoListBox_editTextChanged(const QString &arg1);

private:
	void fillPortsParameters();
	void fillPortsInfo();
	void updateSettings();

	void showAdvanced(bool checked);


};

#endif // SERIALSETTINGSDIALOG_H
