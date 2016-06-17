#include "FaceWidget.hpp"
#include "ui_FaceWidget.h"

#include "basic/Settings.hpp"
#include "agent/Agent.hpp"
#include <QScrollBar>



FaceWidget::FaceWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::FaceWidget)
{
	ui->setupUi(this);
}

FaceWidget::~FaceWidget()
{
	delete ui;
}


void FaceWidget::updateVisibility(){
	Agent *agent=ui->widgetRealtimeValues->getAgent();
	Settings *s=(nullptr!=agent)?&agent->getSettings():nullptr;
	if(nullptr!=s){
		ui->widgetEyes->setVisible(s->getCustomSettingBool("octomy.face"));
		ui->logScroll->setVisible(s->getCustomSettingBool("octomy.debug.log"));
		ui->widgetRealtimeValues->setVisible(s->getCustomSettingBool("octomy.debug.stats"));
		//ui->widgetIdenticon->setVisible(s.getCustomSettingBool("octomy.identicon"));
	}
}


void FaceWidget::appendLog(const QString& text){
	ui->logScroll->appendLog(text);
}


void FaceWidget::setAgent(Agent *a){
	ui->widgetRealtimeValues->setAgent(a);
}
