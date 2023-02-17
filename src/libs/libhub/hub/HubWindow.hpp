#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP

#include "Hub.hpp"
#include "app/log/LogDestination.hpp"
#include "node/NodeWindow.hpp"
#include "components/TryToggleState.hpp"

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
class IdenticonWidget;
class IrisWidget;

class HubWindow: public NodeWindow, public LogDestination
{
	Q_OBJECT
private:
	Ui::HubWindow *ui;
	QSharedPointer<Hub> mHub;
	QTimer mSummaryTimer;
	quint64 mLastTime;
	qreal mAngle=0.0;
	GaitController<qreal> *mGait;
	ZBarScanner *mScanner;

	//QStandardItemModel simClients;
	const QDir topDir;
	QDir lastDir;
	QDir widgetIllustrationOutputDir;

public:
	explicit HubWindow(QWidget *parent = nullptr);
	virtual ~HubWindow();

public:
	void configure() Q_DECL_OVERRIDE;

public:
	QSharedPointer<Hub> hub();

public:
	void appendLog(const QString& text) Q_DECL_OVERRIDE;

private:
	void updateIdentityWidgets();
	void startProcess(QString base);

	void generateTriggers();

#ifdef OC_USE_LIB_EXT_OPENCL

private slots:
	void onGLWidgetInitialized();
#endif
// OC_USE_LIB_EXT_OPENCL

	QString saveIdenticonWidget(IdenticonWidget *iw, QString base="identicon");
	QString saveIrisWidget(IrisWidget *iw, quint32 irisIndex=0, QString base="iris");

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
	void on_pushButtonWidgetIllustrationBrowse_clicked();
	void on_pushButtonWidgetIllustrationSaveAll_clicked();
};

#endif
// HUBWINDOW_HPP
