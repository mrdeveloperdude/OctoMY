#ifndef CAMERAPAIRINGWIDGET_HPP
#define CAMERAPAIRINGWIDGET_HPP

#include "security/PortableID.hpp"

#include <QWidget>
#include <QTimer>

//#include <QCamera>

namespace Ui
{
class CameraPairingWidget;
}



class QVideoProbe;
class PoorMansProbe;
class ZBarScanner;
class QVideoFrame;
class QVideoWidget;
class KeyStore;

class CameraPairingWidget : public QWidget
{
	Q_OBJECT
private:

	Ui::CameraPairingWidget *ui;

	QTimer countDownTimer;
	qint64 badgeDisableTime;
	qint64 scanDisableTime;
	static const qint64 TIMEOUT;

	// TODO: Rework this with new qmultimedia primitives for Qt6
	//QCamera *mCamera;
	
	//QVideoProbe *mVideoProbe;
	//PoorMansProbe *mPoorVideoProbe;
	ZBarScanner *mZbar;

	QVideoWidget *mViewfinder;

public:
	explicit CameraPairingWidget(QWidget *parent = nullptr);
	~CameraPairingWidget();

private:
	PortableID getMyPortableID();

private slots:
	void onCountDownTimeout();
	void detectBarcodes(const QVideoFrame &);
	void onKeystoreReady(bool);
	// TODO: Rework this with new qmultimedia primitives for Qt6
	//void cameraStatusUpdated(QCamera::Status);

private slots:
	void on_pushButtonBadge_toggled(bool checked);
	void on_pushButtonScanner_toggled(bool checked);

};

#endif
// CAMERAPAIRINGWIDGET_HPP
