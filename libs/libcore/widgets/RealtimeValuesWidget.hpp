#ifndef REALTIMEVALUESWIDGET_HPP
#define REALTIMEVALUESWIDGET_HPP

#include <QWidget>

namespace Ui {
	class RealtimeValuesWidget;
}


class QGeoPositionInfo;

class QCompassReading;

class QAccelerometerReading;

class QGyroscopeReading;
class QGyroscopeReading;


class RealtimeValuesWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit RealtimeValuesWidget(QWidget *parent = 0);
		~RealtimeValuesWidget();

	private:
		Ui::RealtimeValuesWidget *ui;

	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);

};

#endif // REALTIMEVALUESWIDGET_HPP
