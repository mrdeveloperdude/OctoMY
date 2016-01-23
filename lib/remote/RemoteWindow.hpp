#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include <QWidget>
#include <QGeoPositionInfo>

#include "widgets/TryToggle.hpp"
#include "basic/LogDestination.hpp"

class Remote;
class QAccelerometerReading;
class QCompassReading;
class QGyroscopeReading;
class CommsChannel;


namespace Ui {
	class RemoteWindow;
}

class RemoteWindow : public QWidget, public LogDestination{
		Q_OBJECT

	private:
		Ui::RemoteWindow *ui;
		Remote *remote;

	public:
		explicit RemoteWindow(Remote *remote, QWidget *parent = 0);
		virtual ~RemoteWindow();


		void appendLog(const QString& text);

		bool eventFilter(QObject *object, QEvent *event);


	private:
		void notifyAndroid(QString);
		void toastAndroid(QString);

	public:

		virtual void keyReleaseEvent(QKeyEvent *);

	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);
		void onTryToggleConnectionChanged(TryToggleState);

		void on_pushButtonConfirmQuit_clicked();
		void on_pushButtonTest_clicked();
};

#endif // REMOTEWINDOW_HPP
