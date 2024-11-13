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
	bool mManualSaveMode{false};

public:

	explicit SerialSettingsWidget(QWidget *parent = nullptr);
	~SerialSettingsWidget();

	void configure(bool manualSaveMode=true, SerialSettings defaults=SerialSettings());
	SerialSettings settings() const;
	void setSettings(SerialSettings &settings);

private:
	// If in auto save mode, triggers save. If not does nothing
	void registerChange();
	void checkCustomBaudRatePolicy(int idx);
	void checkCustomDevicePath(int idx);
	void showPortInfo(int idx);
	
public slots:
	void onSerialDevicesChanged();
	
private slots:
	void applySettings();
	void baudRateChanged(int idx);
	void serialPortChanged(int idx);
	
	void selectDefaults();
	void toggleAdvanced(bool checked);
	void dataBitsChanged(int index);
	void parityChanged(int index);
	void stopBitsChanged(int index);
	void flowControlChanged(int index);
	void serialPortTextChanged(const QString &arg1);
	
private:
	void fillPortsParameters();
	void fillPortsInfo();
	void updateSettings();
	void showAdvanced(bool checked);
	
signals:
	void settingsChanged();

};

#endif
// SERIALSETTINGSDIALOG_H
