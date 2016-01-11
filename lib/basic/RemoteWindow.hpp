#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include <QWidget>
#include <QGeoPositionInfo>

class Remote;
class QAccelerometerReading;
class QCompassReading;
class QGyroscopeReading;

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


	public:

virtual void keyReleaseEvent(QKeyEvent *);

	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);

		void on_pushButtonStart_clicked();
		void on_pushButtonConfirmQuit_clicked();
};

#endif // REMOTEWINDOW_HPP
