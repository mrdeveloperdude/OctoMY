#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP


#include "widgets/TryToggle.hpp"
#include "comms/CommsChannel.hpp"
#include "plot/StatsWindow.hpp"
#include "basic/LogDestination.hpp"
#include "hub/Hub.hpp"
#include "hw/actuators/HexySerial.hpp"


#include <QMainWindow>
#include <QHostInfo>
#include <QCommandLineParser>


namespace qmapcontrol{
	class MapControl;
}

namespace Ui {
	class HubWindow;
}

template<typename T>
class GaitController;

class HubWindow : public QMainWindow, public LogDestination{
		Q_OBJECT
	private:
		Ui::HubWindow *ui;
		Hub *hub;
		QTimer summaryTimer;
		QTimer hexyTimer;
		StatsWindow stats;
		quint64 lastTime;
		qreal angle=0.0f;

		QCPDataMap graphRTT;
		QCPDataMap graphSent;
		QCPDataMap graphReceived;
		QCPDataMap graphLost;
		QCPDataMap graphAcked;
		QCPDataMap graphSendBW;
		QCPDataMap graphReceiveBW;

		double randomWalk;

		HexySerial *hexy;
		GaitController<qreal> *m_gait;

		//QStandardItemModel simClients;


	public:
		explicit HubWindow(Hub *hub, QWidget *parent = 0);
		~HubWindow();

	public:
		void appendLog(const QString& text);


	private:

		void startProcess(QString base);

		void appendGraphData(double rtt, int sent,int received,int lost,int acked, float sendBW,float receiveBW);


	private slots:
		void onListenStateChanged(TryToggleState);
		void onReceivePacket(QSharedPointer<QDataStream> ,QHostAddress ,quint16 );
		void onError(QString msg);
		void onClientAdded(Client *c);
		void onLocalHostLookupComplete(QHostInfo hi);
		void onRemoteHostLookupComplete(QHostInfo hi);
		void onSummaryTimer();
		void onHexyTimer();
		void onHexySettingsChanged();
		void onConnectionStatusChanged(bool);

		void on_pushButtonSendData_clicked();
		void on_pushButtonShowStats_clicked();
		void on_comboBoxAddLocal_currentIndexChanged(const QString &arg1);
		void on_tabWidget_currentChanged(int index);
		void on_pushButtonTest_clicked();
		void on_lineEditQR_textChanged(const QString &arg1);
		void on_pushButtonGenerateKeyPair_clicked();
		void on_pushButtonTestKeyPair_clicked();

		void on_pushButtonUDPPunch_clicked();
};

#endif // HUBWINDOW_HPP
