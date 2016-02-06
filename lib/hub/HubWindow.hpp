#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP


#include "widgets/TryToggle.hpp"
#include "comms/CommsChannel.hpp"
#include "plot/StatsWindow.hpp"
#include "basic/LogDestination.hpp"
#include "hub/Hub.hpp"


#include <QMainWindow>
#include <QHostInfo>
#include <QCommandLineParser>


namespace qmapcontrol{
	class MapControl;
}

namespace Ui {
	class HubWindow;
}

class HubWindow : public QMainWindow, public LogDestination{
		Q_OBJECT
	private:
		Ui::HubWindow *ui;
		Hub *hub;
		QTimer summaryTimer;
		QTimer gaugeTimer;
		StatsWindow stats;
		quint64 startTime;

		QCPDataMap graphRTT;
		QCPDataMap graphSent;
		QCPDataMap graphReceived;
		QCPDataMap graphLost;
		QCPDataMap graphAcked;
		QCPDataMap graphSendBW;
		QCPDataMap graphReceiveBW;

		double randomWalk=0.0f;

		qmapcontrol::MapControl *mc=0;

		//QStandardItemModel simClients;


	public:
		explicit HubWindow(Hub *hub, QWidget *parent = 0);
		~HubWindow();

	public:
		void appendLog(const QString& text);


	private:


		void prepareMap();
		void homeMap();

		void appendGraphData(double rtt, int sent,int received,int lost,int acked, float sendBW,float receiveBW);


	private slots:
		void onListenStateChanged(TryToggleState);
		void onReceivePacket(QSharedPointer<QDataStream> ,QHostAddress ,quint16 );
		void onError(QString msg);
		void onClientAdded(Client *c);
		void onLocalHostLookupComplete(QHostInfo hi);
		void onRemoteHostLookupComplete(QHostInfo hi);
		void onGaugeTimer();
		void onSummaryTimer();

		void on_pushButtonSendData_clicked();
		void on_pushButtonShowStats_clicked();
		void on_comboBoxAddLocal_currentIndexChanged(const QString &arg1);
		void on_tabWidget_currentChanged(int index);
		void on_pushButtonOpenModel_clicked();
};

#endif // HUBWINDOW_HPP
