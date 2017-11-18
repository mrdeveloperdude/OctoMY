#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP


#include "widgets/TryToggle.hpp"
#include "comms/CommsChannel.hpp"

#include "basic/LogDestination.hpp"
#include "Hub.hpp"

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
	QSharedPointer<Hub> mHub;
	QTimer mSummaryTimer;
	quint64 mLastTime;
	qreal mAngle=0.0f;
	GaitController<qreal> *mGait;
	ZBarScanner *mScanner;

	//QStandardItemModel simClients;


public:
	explicit HubWindow(QSharedPointer<Hub> hub, QWidget *parent = 0);
	virtual ~HubWindow();

public:
	void appendLog(const QString& text);


private:

	void updateIdentityWidgets();

	void startProcess(QString base);
#ifdef EXTERNAL_LIB_OPENCL

private slots:
	void onGLWidgetInitialized();
#endif // EXTERNAL_LIB_OPENCL


	// CommsChannel slots
public slots:
	//		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
	void onCommsError(QString message);
	void onCommsClientAdded(CommsSession *c);
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);



private slots:
	void onListenStateChanged(const TryToggleState last, const TryToggleState current);

	void onRemoteHostLookupComplete(QHostInfo hi);
	void onSummaryTimer();

	void onQRRedraw();

private slots:
	void on_pushButtonSendData_clicked();
	void on_comboBoxAddLocal_currentIndexChanged(const QString &arg1);
	void on_tabWidget_currentChanged(int index);
	void on_pushButtonTest_clicked();
	void on_lineEditQR_textChanged(const QString &arg1);

	void on_pushButtonRandomIdenticonID_clicked();
	void on_lineEditIdenticonID_textChanged(const QString &arg1);

	void on_pushButtonSaveIdenticonAgent_clicked();
	void on_horizontalSliderIrisIndex_valueChanged(int value);
	void on_pushButtonSaveIdenticonIris_clicked();
	void on_pushButtonSaveIdenticonRemote_clicked();
	void on_pushButtonSaveIdenticonHub_clicked();
};

#endif // HUBWINDOW_HPP
