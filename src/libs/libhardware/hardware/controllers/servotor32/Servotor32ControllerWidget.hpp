#ifndef SERVOTOR32CONTROLLERWIDGET_HPP
#define SERVOTOR32CONTROLLERWIDGET_HPP

#include "HexySerial.hpp"
#include "gait/GaitController.hpp"
#include "components/TryToggleState.hpp"

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
	qreal pos[HexySerial::SERVO_COUNT]= {0.0};
	GaitController<qreal> gait;
	QTimer gaitTimer;
	Servotor32Controller *mController;

public:
	explicit Servotor32ControllerWidget(QWidget *parent = nullptr);
	~Servotor32ControllerWidget();

public:
	void configure(Servotor32Controller *);

private:
	void setUILock(bool);

private slots:
	void onUpdateGaitTimer();
	void onConnectChanged(const TryToggleState, const TryToggleState);
	void onLimbIKUpdated();
	void onHexySettingsChanged();
	void onHexyConenctionChanged();
	void on_pushButtonDisableAll_clicked();
	void on_pushButtonGait_toggled(bool checked);
	void on_dialFeedrate_valueChanged(int value);

public slots:
	void onServoMoved(quint32 id, qreal pos);
	void onServoKilled(quint32 id);

};

#endif
// SERVOTOR32CONTROLLERWIDGET_HPP
