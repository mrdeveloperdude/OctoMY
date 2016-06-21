#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "widgets/TryToggle.hpp"
#include "basic/LogDestination.hpp"


#include <QWidget>
#include <QGeoPositionInfo>
#include <QMenu>

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
		QMenu menu;

	public:
		explicit AgentWindow(Agent *agent, QWidget *parent = 0);
		virtual ~AgentWindow();


		void appendLog(const QString& text);

		void updateVisibility();


	private:
		void notifyAndroid(QString);
		void toastAndroid(QString);

	public:

		virtual void keyReleaseEvent(QKeyEvent *);

	private slots:

		void onStartCameraPairing();
		void onStartPairing();
		void onStartPlanEditor();
	private slots:

		void onFaceVisibilityChanged(bool);
		void onLogVisibilityChanged(bool);
		void onStatsVisibilityChanged(bool);


		void onTryToggleConnectionChanged(TryToggleState);

	private slots:
		void on_pushButtonConfirmQuit_clicked();
		void on_pushButtonBack_clicked();
		void on_pushButtonMenu_clicked();
};


#endif // AGENTWINDOW_HPP


