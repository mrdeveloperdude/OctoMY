#ifndef ARDUMYCONTROLLERWIDGET_HPP
#define ARDUMYCONTROLLERWIDGET_HPP

#include "widgets/TryToggleState.hpp"

#include <QWidget>

class ArduMYController;
class SerialSettings;

namespace Ui
{
class ArduMYControllerWidget;
}

class ArduMYControllerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ArduMYControllerWidget *ui;
	ArduMYController *mController;

public:
	explicit ArduMYControllerWidget(QWidget *parent = 0);
	~ArduMYControllerWidget();

public:
	void configure(ArduMYController *controller);

	void setSerialSettings(SerialSettings &s);

private:

	void setUILock(bool lock);

	void reconnectActuatorWidgets();

	void showQuickGuide(bool show);

public slots:

	void onSerialSettingsChanged();
	void onConnectionChanged();
	void onTryConnectChanged(const TryToggleState last, const TryToggleState current);

private slots:
	void on_pushButtonConfigureSerial_clicked();
	void on_pushButtonConfigureActuators_clicked();

	void on_comboBoxAddActuator_currentIndexChanged(int index);
	void on_pushButtonSync_clicked();


	void on_pushButtonShowQuickGuide_toggled(bool checked);
};

#endif // ARDUMYCONTROLLERWIDGET_HPP
