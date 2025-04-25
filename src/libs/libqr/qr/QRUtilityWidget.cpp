#include "QRUtilityWidget.hpp"
#include "ui_QRUtilityWidget.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "zbar/ZBarScanner.hpp"

#include <QPainter>

QRUtilityWidget::QRUtilityWidget(
	QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::QRUtilityWidget)
{
	ui->setupUi(this);
}

QRUtilityWidget::~QRUtilityWidget()
{
	delete ui;
	delete mScanner;
	mScanner=nullptr;
}



void QRUtilityWidget::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	auto s = node->settings();
	
	if(!connect(ui->widgetQR, SIGNAL(qrRedraw()), this, SLOT(onQRRedraw())) ) {
		qWarning()<<"ERROR: Could not connect";
	}
}



void QRUtilityWidget::onQRRedraw()
{
	OC_METHODGATE();
	if(nullptr==mScanner) {
		mScanner=OC_NEW ZBarScanner();
	}
	const QPixmap &px=ui->widgetQR->qrPixmap();
	if(nullptr!=mScanner) {
		QList<ZScanResult> found=mScanner->scan(px);
		int fsz=found.size();
		qDebug()<<"fsz="<<fsz;
		if(fsz<1) {
			ui->labelQR->setText("ERROR: no QR code found!");
			ui->labelQRScanner->setPixmap(QPixmap());
		} else if(fsz>1) {
			ui->labelQR->setText("ERROR: found more than one QR code!");
			ui->labelQRScanner->setPixmap(QPixmap());
		} else {
			QString data=found[0].data;
			if(data.size()<=0) {
				ui->labelQR->setText("ERROR: byte segments not currently supported!");
				ui->labelQRScanner->setPixmap(QPixmap());
			} else {
				ui->labelQR->setText(data);
				QPixmap px2(px);
				QPainter p(&px2);
				found[0].paint(p);
				ui->labelQRScanner->setPixmap(px2);
			}
		}
	} else {
		ui->labelQR->setText("No scanner");
		qWarning()<<"PX: "<<px<<" scanner: "<<mScanner;
	}
}



void QRUtilityWidget::qrTextChanged(const QString &text)
{
	OC_METHODGATE();
	// Initialize scanner
	ui->labelQR->setText("Working...");
	ui->labelQRScanner->setPixmap(QPixmap());
	//Initialize generator
	ui->widgetQR->setQRData(text);
}

