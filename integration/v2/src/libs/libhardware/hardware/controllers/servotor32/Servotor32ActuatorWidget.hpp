#ifndef SERVOTOR32ACTUATORWIDGET_HPP
#define SERVOTOR32ACTUATORWIDGET_HPP

#include <QWidget>

namespace Ui
{
class Servotor32ActuatorWidget;
}

class Settings;

const int INITIAL_SERVO_TRIM=150;

class Servotor32ActuatorWidget: public QWidget
{
	Q_OBJECT
private:
	Ui::Servotor32ActuatorWidget *ui;
	static quint32 s_sid;
	quint32 m_id;
	int m_low_trim;
	int m_high_trim;
	Settings *settings;

public:
	explicit Servotor32ActuatorWidget(QWidget *parent = nullptr);
	virtual ~Servotor32ActuatorWidget();

public:

	void configure(Settings *settings, quint32 id, int low_trim=INITIAL_SERVO_TRIM, int high_trim=INITIAL_SERVO_TRIM);

	void reconfigureTrim();

	void disableServo();
signals:
	void servoMoved(quint32 id, qreal val);
	void servoKilled(quint32 id);

private slots:
	void onServoMoved();
	void onServoKilled();
private slots:
	void on_pushButtonCenter_clicked();

	void on_pushButton_toggled(bool checked);
	void on_spinBoxLowTrim_valueChanged(int arg1);
	void on_spinBoxHighTrim_valueChanged(int arg1);
};

#endif
// SERVOTOR32ACTUATORWIDGET_HPP
