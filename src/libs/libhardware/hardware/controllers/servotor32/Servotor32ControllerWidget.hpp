#ifndef SERVOTOR32CONTROLLERWIDGET_HPP
#define SERVOTOR32CONTROLLERWIDGET_HPP

#include "Servotor32Controller.hpp"
#include "components/TryToggleState.hpp"
#include "gait/GaitController.hpp"

#include <QWidget>

namespace Ui
{
class Servotor32ControllerWidget;
}


class Servotor32Controller;


class Servotor32ControllerWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::Servotor32ControllerWidget *ui;
	qreal mPos[32] = {0.0};
	Servotor32Controller *mController;

public:
	explicit Servotor32ControllerWidget(QWidget *parent = nullptr);
	~Servotor32ControllerWidget();

public:
	void configure(Servotor32Controller *);

private:
	void setUILock(bool);
	void setHookActuators(bool);
	void setHookController(bool);
	void setHookConnectButton(bool);
	void setHookSerialSettings(bool);
	void appendNotice(const QString &notice);

private slots:
	void onConnectChanged(const TryToggleState, const TryToggleState);
	void onSettingsChanged();
	void onSerialSettingsChanged();
	void onConnectionChanged();
	void onErrorOccurred(const QString &error);

	void limpAll();

public slots:
	void onServoMoved(quint32 id, qreal pos);
	void onServoLimped(quint32 id);

};

#endif
// SERVOTOR32CONTROLLERWIDGET_HPP
