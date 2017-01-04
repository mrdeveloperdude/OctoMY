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


namespace qmapcontrol
{
class MapControl;
}

namespace Ui
{
class HubWindow;
}

template<typename T>
class GaitController;

class ZBarScanner;

class HubWindow : public QMainWindow, public LogDestination
{
	Q_OBJECT
private:
	Ui::HubWindow *ui;
	Hub *mHub;
	QTimer mSummaryTimer;
	quint64 mLastTime;
	qreal mAngle=0.0f;
	GaitController<qreal> *mGait;
	ZBarScanner *mScanner;

	//QStandardItemModel simClients;


public:
	explicit HubWindow(Hub *mHub, QWidget *parent = 0);
	virtual ~HubWindow();

public:
	void appendLog(const QString& text);


private:

	void startProcess(QString base);
#ifdef EXTERNAL_LIB_OPENCL

private slots:
	void onGLWidgetInitialized();
#endif


	// CommsChannel slots
public slots:
	//		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
	void onCommsError(QString message);
	void onCommsClientAdded(CommsSession *c);
	void onCommsConnectionStatusChanged(bool c);



private slots:
	void onListenStateChanged(const TryToggleState last, const TryToggleState current);
	void onLocalHostLookupComplete(QHostInfo hi);
	void onRemoteHostLookupComplete(QHostInfo hi);
	void onSummaryTimer();

	void onQRRedraw();


	void on_pushButtonSendData_clicked();
	void on_comboBoxAddLocal_currentIndexChanged(const QString &arg1);
	void on_tabWidget_currentChanged(int index);
	void on_pushButtonTest_clicked();
	void on_lineEditQR_textChanged(const QString &arg1);

	void on_pushButtonRandomIdenticonID_clicked();
	void on_lineEditIdenticonID_textChanged(const QString &arg1);

};

#endif // HUBWINDOW_HPP
