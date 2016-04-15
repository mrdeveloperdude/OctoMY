#ifndef PAIRINGWIDGET_HPP
#define PAIRINGWIDGET_HPP


#include "security/PortableID.hpp"

#include <QWidget>
#include <QTimer>
#include <QCamera>

namespace Ui {
	class PairingWidget;
}



class QVideoProbe;
class PoorMansProbe;
class ZBarScanner;
class QVideoFrame;
class QVideoWidget;
class KeyStore;

class PairingWidget : public QWidget
{
		Q_OBJECT
	private:

		Ui::PairingWidget *ui;

		QTimer countDownTimer;
		qint64 badgeDisableTime;
		qint64 scanDisableTime;
		static const qint64 TIMEOUT;


		QCamera *camera;
		QVideoProbe *videoProbe;
		PoorMansProbe *poorVideoProbe;
		ZBarScanner *zbar;

		QVideoWidget *viewfinder;

		KeyStore &keystore;


	public:
		explicit PairingWidget(QWidget *parent = 0);
		~PairingWidget();

	private:
		PortableID getMyPortableID();
	private slots:
		void onCountDownTimeout();
		void detectBarcodes(const QVideoFrame &);
		void onKeystoreReady();
		void cameraStatusUpdated(QCamera::Status);

	private slots:
		void on_pushButtonBadge_toggled(bool checked);
		void on_pushButtonScanner_toggled(bool checked);




};

#endif // PAIRINGWIDGET_HPP
