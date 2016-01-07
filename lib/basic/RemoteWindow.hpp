#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include <QWidget>
#include <QGeoPositionInfo>

class Remote;
class QAccelerometerReading;
class QCompassReading;

namespace Ui {
	class RemoteWindow;
}

class RemoteWindow : public QWidget{
		Q_OBJECT

	private:
		Ui::RemoteWindow *ui;
		Remote *remote;

	public:
		explicit RemoteWindow(Remote *remote, QWidget *parent = 0);
		virtual ~RemoteWindow();


	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);

};

#endif // REMOTEWINDOW_HPP
