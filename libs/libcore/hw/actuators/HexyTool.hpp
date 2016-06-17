#ifndef HEXYTOOL_HPP
#define HEXYTOOL_HPP

#include "widgets/TryToggle.hpp"
#include "hw/actuators/HexySerial.hpp"
#include "puppet/GaitController.hpp"
#include <QWidget>


namespace Ui {
	class HexyTool;
}

class Settings;

class HexyTool : public QWidget
{
		Q_OBJECT
	private:
		Ui::HexyTool *ui;
		HexySerial *serial;
		qreal pos[HexySerial::SERVO_COUNT]={0.0};
		GaitController<qreal> gait;
		QTimer gaitTimer;
		Settings *settings;

	public:
		explicit HexyTool(QWidget *parent = 0);
		~HexyTool();

	public:

		void configure(Settings *settings);

		void killAll();
	private slots:
		void onUpdateGaitTimer();
		void onConnectChanged(TryToggleState);
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

#endif // HEXYTOOL_HPP
