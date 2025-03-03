#include "UtilitiesWidget.hpp"
#include "ui_UtilitiesWidget.h"

#include "hexedit/QHexEdit.hpp"
#include "hexedit/QHexEditData.hpp"
#include "zbar/ZBarScanner.hpp"

#include "log/LogDevice.hpp"

#include "app/Settings.hpp"
#include "hub/Hub.hpp"

UtilitiesWidget::UtilitiesWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::UtilitiesWidget)
	, mScanner(nullptr)
	, mLogDevice{OC_NEW LogDeviceNoop}
{
	ui->setupUi(this);
}

UtilitiesWidget::~UtilitiesWidget()
{
	delete ui;
	delete mScanner;
	mScanner=nullptr;
	
}




QSharedPointer<Hub> UtilitiesWidget::hub(){
	return mHub;
}


void UtilitiesWidget::configure(QSharedPointer<Settings> _settings, QSharedPointer<Hub> hub){
	OC_METHODGATE();
	mHub = hub;
	auto settings = _settings.data();
	auto h = this->hub();
	auto cmd = h->context()->commandLine();
	
	
	ui->tabWidgetUtilities->setCurrentIndex(0);
	
	ui->lineEditRemoteAddress->configure(settings, "localhost","hub-listen_address","The address of the remote host");
	ui->lineEditRemotePort->configure(settings, "","hub-port","The port of the remote host");
	
	ui->lineEditRemotePort->setText(cmd->remotePortOption.valueName());
	qDebug()<<"OVERRIDING REMOTE PORT WITH VALUE FROM CMDLINE: "<<ui->lineEditRemotePort->text();
	
	
	ui->lineEditRemotePort->setValidator( OC_NEW QIntValidator(0, 65535, this) );
	
	QByteArray ba("HELLO WORLD");
	QHexEditData* hexdata = QHexEditData::fromMemory(ba);
	ui->hexEditor->setData(hexdata);
	
	
	
	if(!connect(ui->widgetQR, SIGNAL(qrRedraw()), this, SLOT(onQRRedraw()), OC_CONTYPE) ) {
		qWarning()<<"ERROR: Could not connect";
	}
	
	
}



void UtilitiesWidget::onQRRedraw()
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
		qWarning()<<"PX: "<<px<<" scanner: "<<mScanner;
	}
}



void UtilitiesWidget::on_lineEditQR_textChanged(const QString &text)
{
	OC_METHODGATE();
	// Initialize scanner
	ui->labelQR->setText("Working...");
	ui->labelQRScanner->setPixmap(QPixmap());
	//Initialize generator
	ui->widgetQR->setQRData(text);
}




void UtilitiesWidget::onRemoteHostLookupComplete(QHostInfo hi)
{
	OC_METHODGATE();
	qDebug()<<"## onRemoteHostLookupComplete";
	for(QHostAddress adr:hi.addresses()) {
		if(adr.isNull()) {
			mLogDevice->appendLog("Skipping invalid address during remote host lookup: "+adr.toString());
		} else {
			const int l=ui->horizontalSliderSendCount->value();
			const int sz=int(500*ui->horizontalSliderSendSize->value())/100;
			for(int i=0; i<l; ++i) {
				QByteArray ba=("PING "+QString::number(i)+"/"+QString::number(l)).toUtf8()+" ";
				const int sz2=sz-ba.size();
				for(int i=0; i<sz2; ++i) {
					ba.append("X");
				}
				//				quint64 port=ui->lineEditRemotePort->text().toInt();
				//			CommsChannel *comms=hub->getComms();
				//TODO: convert this to use couriers
				
				//	comms->sendPackage(ba,adr,port);
			}
			mLogDevice->appendLog( "SENDING "+QString::number(l)+" DATA PACKETS OF SIZE "+QString::number(sz)+" to "+ui->lineEditRemoteAddress->text()+ "("+adr.toString()+"):"+ui->lineEditRemotePort->text());
			return;
		}
	}
	mLogDevice->appendLog("No valid addresses found during remote host lookup, aborting send");
}

