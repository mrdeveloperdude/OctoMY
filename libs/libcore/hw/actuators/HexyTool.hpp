#ifndef HEXYTOOL_HPP
#define HEXYTOOL_HPP

#include "widgets/TryToggle.hpp"
#include "hw/actuators/HexySerial.hpp"

#include <QWidget>


namespace Ui {
	class HexyTool;
}

class HexyTool : public QWidget
{
		Q_OBJECT
	private:
		Ui::HexyTool *ui;
		HexySerial *hexy;
		qreal pos[HexySerial::SERVO_COUNT]={0.0};
	public:
		explicit HexyTool(QWidget *parent = 0);
		~HexyTool();


	private slots:
		void onConnectChanged(TryToggleState);
		void onHexySettingsChanged();
		void onServoPositionChanged();
		void on_pushButtonCenter_clicked();
};

#endif // HEXYTOOL_HPP
