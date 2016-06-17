#include "RealtimeValuesWidget.hpp"
#include "ui_RealtimeValuesWidget.h"

#include "agent/Agent.hpp"


#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


RealtimeValuesWidget::RealtimeValuesWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::RealtimeValuesWidget)
	, agent(nullptr)
{
	ui->setupUi(this);
	resetView();


}



void RealtimeValuesWidget::resetView(){
	ui->labelLocal->setText("N/A");
	ui->labelControl->setText("N/A");
	ui->labelGPS->setText("N/A");
	ui->labelCompass->setText("N/A");
	ui->labelGyroscope->setText("N/A");
	ui->labelAccelerometer->setText("N/A");

	ui->labelLocal->setVisible(false);
	ui->labelNetworkSymbol->setVisible(false);
	ui->labelControl->setVisible(false);


	ui->labelGPSSymbol->setVisible(false);
	ui->labelGPS->setVisible(false);

	ui->labelCompassSymbol->setVisible(false);
	ui->labelCompass->setVisible(false);

	ui->labelGyroscopeSymbol->setVisible(false);
	ui->labelGyroscope->setVisible(false);

	ui->labelAccelerometerSymbol->setVisible(false);
	ui->labelAccelerometer->setVisible(false);
}

void RealtimeValuesWidget::setAgent(Agent *a){
	if(agent!=a){
		if(nullptr!=agent){
			agent->unHookSensorSignals(*this);
		}
		agent=a;
		if(nullptr!=agent){
			agent->hookSensorSignals(*this);
		}
		resetView();
	}
}


Agent *RealtimeValuesWidget::getAgent(){
	return agent;
}

RealtimeValuesWidget::~RealtimeValuesWidget()
{
	delete ui;
}



void RealtimeValuesWidget::onPositionUpdated(const QGeoPositionInfo &info){
	ui->labelGPS->setText(QString("<%1, %2>").arg(info.coordinate().latitude(),0,'g',5).arg(info.coordinate().longitude(),0,'g',5));
	//appendLog("GPS update: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
	ui->labelGPSSymbol->setVisible(true);
	ui->labelGPS->setVisible(true);
}


void RealtimeValuesWidget::onCompassUpdated(QCompassReading *r){
	ui->labelCompass->setText(QString("%1").arg(r->azimuth(),0,'g',2));
	ui->labelCompassSymbol->setVisible(true);
	ui->labelCompass->setVisible(true);
}

void RealtimeValuesWidget::onAccelerometerUpdated(QAccelerometerReading *r){
	ui->labelAccelerometer->setText(QString("<%1, %2, %3>").arg(r->x(),0,'g',2).arg(r->y(),0,'g',2).arg(r->z(),0,'g',2) );
	ui->labelAccelerometerSymbol->setVisible(true);
	ui->labelAccelerometer->setVisible(true);

}

void RealtimeValuesWidget::onGyroscopeUpdated(QGyroscopeReading *r){
	ui->labelGyroscope->setText(QString("<%1, %2, %3>").arg(r->x(),0,'g',2).arg(r->y(),0,'g',2).arg(r->z(),0,'g',2) );
	ui->labelGyroscopeSymbol->setVisible(true);
	ui->labelGyroscope->setVisible(true);
}

