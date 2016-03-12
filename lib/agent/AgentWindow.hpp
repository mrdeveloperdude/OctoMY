#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP


#include <QWidget>
#include <QGeoPositionInfo>

#include "widgets/TryToggle.hpp"
#include "basic/LogDestination.hpp"

class Agent;
class QAccelerometerReading;
class QCompassReading;
class QGyroscopeReading;
class CommsChannel;
class HexyTool;


namespace Ui {
	class AgentWindow;
}

class AgentWindow : public QWidget, public LogDestination{
		Q_OBJECT

	private:
		Ui::AgentWindow *ui;
		Agent *agent;
		HexyTool *hexy;

	public:
		explicit AgentWindow(Agent *agent, QWidget *parent = 0);
		virtual ~AgentWindow();


		void appendLog(const QString& text);


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
		void on_pushButtonCamera_clicked();
		void on_pushButtonBack_clicked();
};


#endif // AGENTWINDOW_HPP
