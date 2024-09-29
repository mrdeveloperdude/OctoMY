#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP

#include "app/log/LogDestination.hpp"
#include "node/NodeWindow.hpp"


#include <QMainWindow>
#include <QHostInfo>
#include <QCommandLineParser>

class Hub;
class CommsSession;

namespace qmapcontrol
{
class MapControl;
}

namespace Ui
{
class HubWindow;
}


class HubWindow: public NodeWindow, public LogDestination
{
	Q_OBJECT
private:
	Ui::HubWindow *ui;
	QSharedPointer<Hub> mHub;
	quint64 mLastTime;
	qreal mAngle=0.0;

public:
	explicit HubWindow(QWidget *parent = nullptr);
	virtual ~HubWindow();
	
	// NodeWindow
public:
	void configure() override;

public:
	QSharedPointer<Hub> hub();

	// LogDestination
public:
	void appendLog(const QString& text) override;

	// CommsChannel slots
public slots:
	//		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
	void onCommsError(QString message);
	void onCommsClientAdded(CommsSession *c);
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);

private slots:
#ifdef OC_USE_LIB_EXT_OPENCL
	void onGLWidgetInitialized();
#endif
	// OC_USE_LIB_EXT_OPENCL
	void on_pushButtonSendData_clicked();
	void on_pushButtonTest_clicked();

	void on_lineEditIdenticonID_textChanged(const QString &arg1);
};

#endif
// HUBWINDOW_HPP
