#ifndef ACTUATORWIDGET_HPP
#define ACTUATORWIDGET_HPP

#include <QWidget>

namespace Ui {
	class ActuatorWidget;
}

class Settings;

const int INITIAL_SERVO_TRIM=150;

class ActuatorWidget: public QWidget
{
		Q_OBJECT
	private:
		Ui::ActuatorWidget *ui;
		static quint32 s_sid;
		quint32 mID;
		int mLowTrim;
		int mHighTrim;
		Settings *settings;
		quint32 mTabIndex;

	public:
		explicit ActuatorWidget(QWidget *parent = 0);
		virtual ~ActuatorWidget();

	public:

		void configure(Settings *settings, quint32 id, int low_trim=INITIAL_SERVO_TRIM, int high_trim=INITIAL_SERVO_TRIM);

		void reconfigureTrim();

		void updateTabsVisibility();

		void disableServo();
	signals:
		void servoMoved(quint32 id, qreal val);
		void servoKilled(quint32 id);

	private slots:
		void onServoMoved();
		void onServoKilled();
	private slots:
		void on_pushButtonCenter_clicked();

		void on_pushButtonConfigure_toggled(bool checked);
		void on_spinBoxLowTrim_valueChanged(int arg1);
		void on_spinBoxHighTrim_valueChanged(int arg1);
		void on_tabWidget_currentChanged(int index);
		void on_comboBoxActuatorType_currentIndexChanged(int index);
		void on_pushButtonResetGearRatio_clicked();
};

#endif // ACTUATORWIDGET_HPP

