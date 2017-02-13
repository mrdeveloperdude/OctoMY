#include "LocusManagerWidget.hpp"
#include "ui_LocusManagerWidget.h"


#include "../libutil/utility/Standard.hpp"
#include "hw/actuators/WheeledLocusWidget.hpp"


#include <QSpacerItem>


LocusManagerWidget::LocusManagerWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LocusManagerWidget)
{
	ui->setupUi(this);
}

LocusManagerWidget::~LocusManagerWidget()
{
	delete ui;
}




void LocusManagerWidget::configure(quint32 num)
{
	quint32 count=mWidgets.count();
	qDebug()<<"---- COUNT: "<<count<<" NUM: "<<num;
	// Trivial reject, no change
	if(num==count) {
		return;
	} else {
		if(num>count) {
			quint32 end=num;
			num-=count;

			for(quint32 i=0; i<num; ++i) {
				qDebug()<<"ADDING ITeMN "<<i<<" of "<<num<<" to get to "<<end;
				WheeledLocusWidget *si=new WheeledLocusWidget();
				if(nullptr!=si) {
//					si->configure(config);
					/*
					if(!connect(si,SIGNAL(servoMoved(quint32, qreal)),this,SLOT(onServoMoved(quint32, qreal)),OC_CONTYPE)) {
						qWarning()<<"ERROR: could not connect";
					}
					if(!connect(si,SIGNAL(servoKilled(quint32)),this,SLOT(onServoLimped(quint32)),OC_CONTYPE)) {
						qWarning()<<"ERROR: could not connect";
					}
					*/
					mWidgets.push_back(si);
					ui->widgetCompressedContent->layout()->addWidget(si);
					si->setParent(ui->widgetCompressedContent);
					QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
					si->setMinimumSize(0,0);
					si->setSizePolicy(sizePolicy);
					si->updateGeometry();
					si->adjustSize();

				}
			}
		} else {
			for(quint32 i=count; i>num; --i) {
				qDebug()<<"REMOVING ITeMN "<<i<<" of "<<count<<" to get to "<<num;
				QWidget *w=mWidgets.takeFirst();
				ui->widgetCompressedContent->layout()->removeWidget(w);
				w->setParent(nullptr);
				w->deleteLater();
			}
		}
		ui->widgetCompressedContent->updateGeometry();
		ui->widgetCompressedContent->adjustSize();

	}
}



void LocusManagerWidget::onServoMoved(quint32 id, qreal val)
{

}

void LocusManagerWidget::onServoLimped(quint32 id)
{

}
