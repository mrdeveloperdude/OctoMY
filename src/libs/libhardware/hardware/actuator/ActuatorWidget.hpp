#ifndef ACTUATORWIDGET_HPP
#define ACTUATORWIDGET_HPP

#include <QWidget>

#include "uptime/SharedPointerWrapper.hpp"
#include "hardware/controllers/IController.hpp"

//class IController;


namespace Ui
{
class ActuatorWidget;
}


const int INITIAL_SERVO_TRIM=150;

class ActuatorWidget: public QWidget
{
	Q_OBJECT
private:
	Ui::ActuatorWidget *ui;
	ACTUATOR_INDEX mIndex{0};
	ACTUATOR_VALUE mLowTrim{0};
	ACTUATOR_VALUE mHighTrim{0};
	QSharedPointer<IController> mController;

public:
	explicit ActuatorWidget(QWidget *parent = nullptr);
	virtual ~ActuatorWidget();

public:
	void configure(QSharedPointer<IController> controller, ACTUATOR_INDEX index);
	
private:
	bool isEnabled();

private slots:
	void lowTrimUpdated(ACTUATOR_VALUE lowTrim);
	void highTrimUpdated(ACTUATOR_VALUE highTrim);

private slots:
	void move();
	void center();
	void toggleEnabled(bool enabled);
	void toggleReversed(bool reversed);
	void toggleLimp(bool limp);
	void toggleSettings(bool on);
	

signals:
	void moved(ACTUATOR_INDEX id, ACTUATOR_VALUE val);
	void limpChanged(ACTUATOR_INDEX id, bool limp);
	void reversedChanged(ACTUATOR_INDEX id, bool reversed);
	void lowTrimChanged(ACTUATOR_INDEX id, ACTUATOR_VALUE lowTrim);
	void highTrimChanged(ACTUATOR_INDEX id, ACTUATOR_VALUE highTrim);

};

#endif
// ACTUATORWIDGET_HPP
