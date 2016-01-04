#ifndef SERVERWINDOW_HPP
#define SERVERWINDOW_HPP


#include "widgets/TryToggle.hpp"


#include <QMainWindow>
#include <QHostInfo>
#include <QCommandLineParser>

#include "comms/Server.hpp"

#include "plot/StatsWindow.hpp"

#include "basic/LogDestination.hpp"

namespace qmapcontrol{
	class MapControl;
}

namespace Ui {
	class ServerWindow;
}

class ServerWindow : public QMainWindow, public LogDestination{
		Q_OBJECT
	private:
		Server *cs;
		Ui::ServerWindow *ui;
		QTimer summaryTimer;
		StatsWindow stats;

		QCPDataMap graphRTT;
		QCPDataMap graphSent;
		QCPDataMap graphReceived;
		QCPDataMap graphLost;
		QCPDataMap graphAcked;
		QCPDataMap graphSendBW;
		QCPDataMap graphReceiveBW;

		double randomWalk=0.0f;


		int lastSentPackets=0;
		int lastReceivedPackets=0;
		int lastLostPackets=0;
		int lastAckedPackets=0;

		QCommandLineParser &opts;
		qmapcontrol::MapControl *mc=0;

		QStandardItemModel simClients;


	public:
		explicit ServerWindow(QCommandLineParser &opts, Server *server, QWidget *parent = 0);
		~ServerWindow();

	public:
		void appendLog(const QString& text);


	private:

		void prepareMap();
		void updateClientsList();
		void homeMap();
		void prepareLocalAddresses();

		void appendGraphData(double rtt, int sent,int received,int lost,int acked, float sendBW,float receiveBW);


	private slots:
		void onListenStateChanged(TryToggleState);
		void onReceivePacket(QByteArray ba,QHostAddress host,quint16 port);
		void onError(QString msg);
		void onLocalHostLookupComplete(QHostInfo hi);
		void onRemoteHostLookupComplete(QHostInfo hi);
		void onSummaryTimer();
		void on_pushButtonSendData_clicked();
		void on_pushButtonShowStats_clicked();
		void on_listViewClients_clicked(const QModelIndex &index);
};

#endif // SERVERWINDOW_HPP
