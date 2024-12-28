#ifndef ARDUMYACTUATORWIDGET_HPP
#define ARDUMYACTUATORWIDGET_HPP



#include <QWidget>

#include "hardware/controllers/IController.hpp"

struct ArduMYActuator;
struct ArduMYActuatorConfig;

namespace Ui
{
class ArduMYActuatorWidget;
}

class Settings;

const int INITIAL_SERVO_TRIM=150;

class ArduMYActuatorWidget: public QWidget
{
	Q_OBJECT
private:
	Ui::ArduMYActuatorWidget *ui;
	quint32 mID;
	int mLowTrim;
	int mHighTrim;
	int mTabIndex;
	ArduMYActuator *mActuator;

public:
	explicit ArduMYActuatorWidget(QWidget *parent = nullptr);
	virtual ~ArduMYActuatorWidget();

private:
	void reconfigureTrim();
	void updateTabsVisibility();
	void reset();

public:
	void configure(ArduMYActuator *, quint32 id);
	void disableServo();

signals:
	void actuatorMoved(ACTUATOR_INDEX id, qreal val);
	void actuatorLimped(ACTUATOR_INDEX id, bool l);
	void actuatorDeleted(ACTUATOR_INDEX id);

private slots:
	void onActuatorMoved();
	void onActuatorLimped();

private slots:
	void on_pushButtonCenter_clicked();

	void on_pushButtonConfigure_toggled(bool checked);
	void on_spinBoxLowTrim_valueChanged(int arg1);
	void on_spinBoxHighTrim_valueChanged(int arg1);
	void on_tabWidget_currentChanged(int index);
	void on_comboBoxActuatorType_currentIndexChanged(int index);
	void on_pushButtonResetGearRatio_clicked();
	void on_pushButtonName_clicked();
	void on_comboBoxActuatorRepresentation_currentIndexChanged(int index);
	void on_pushButtonDelete_clicked();
};


#endif
// ARDUMYACTUATORWIDGET_HPP
