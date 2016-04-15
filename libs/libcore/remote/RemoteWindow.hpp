#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include "widgets/TryToggle.hpp"
#include "basic/LogDestination.hpp"


#include <QWidget>
#include <QGeoPositionInfo>
#include <QHostAddress>


class Remote;
class QAccelerometerReading;
class QCompassReading;
class QGyroscopeReading;
class CommsChannel;
class Client;


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

		void updateControlLevel();
		void prepareMap();
		void homeMap();

	public:

		virtual void keyReleaseEvent(QKeyEvent *);

		//CommsChannel slots
	public slots:

		void onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16);
		void onError(QString);
		void onClientAdded(Client *);
		void onConnectionStatusChanged(bool);

		//Internal Sensor slots
	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);

		void onServoPositionChanged(int);

		//Internal UI slots
	private slots:
		void onTryToggleConnectionChanged(TryToggleState);
		void on_pushButtonConfirmQuit_clicked();
		void on_pushButtonTest_clicked();
		void on_comboBoxControlLevel_currentIndexChanged(int index);
		void on_pushButtonControl_clicked();
		void on_pushButtonStatus_clicked();
		void on_pushButtonPair_clicked();
		void on_pushButtonBack_clicked();
};

#endif // REMOTEWINDOW_HPP
