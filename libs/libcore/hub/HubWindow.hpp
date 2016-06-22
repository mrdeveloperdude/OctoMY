#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP


#include "widgets/TryToggle.hpp"
#include "comms/CommsChannel.hpp"

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
		quint64 lastTime;
		qreal angle=0.0f;
		GaitController<qreal> *m_gait;

		//QStandardItemModel simClients;


	public:
		explicit HubWindow(Hub *hub, QWidget *parent = 0);
		virtual ~HubWindow();

	public:
		void appendLog(const QString& text);


	private:

		void startProcess(QString base);



	private slots:
		void onListenStateChanged(TryToggleState);
		void onReceivePacket(QSharedPointer<QDataStream> ,QHostAddress ,quint16 );
		void onError(QString msg);
		void onClientAdded(Client *c);
		void onLocalHostLookupComplete(QHostInfo hi);
		void onRemoteHostLookupComplete(QHostInfo hi);
		void onSummaryTimer();
		void onConnectionStatusChanged(bool);

		void on_pushButtonSendData_clicked();
		void on_pushButtonShowStats_clicked();
		void on_comboBoxAddLocal_currentIndexChanged(const QString &arg1);
		void on_tabWidget_currentChanged(int index);
		void on_pushButtonTest_clicked();
		void on_lineEditQR_textChanged(const QString &arg1);

		void on_pushButtonUDPPunch_clicked();
};

#endif // HUBWINDOW_HPP
