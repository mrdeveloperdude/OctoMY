#include "MtlobeManagerWidget.hpp"
#include "ui_MtlobeManagerWidget.h"


#include "utility/Standard.hpp"
#include "mtlobe/WheeledMtlobeWidget.hpp"
#include "mtlobe/Mtlobe.hpp"

#include "mtlobe/MtlobeController.hpp"


#include <QSpacerItem>


MtlobeManagerWidget::MtlobeManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::MtlobeManagerWidget)
	, mController(nullptr)
{
	ui->setupUi(this);
}

MtlobeManagerWidget::~MtlobeManagerWidget()
{
	delete ui;
}


void MtlobeManagerWidget::addWidget(QWidget &w)
{
	qDebug()<<"--- ADDING WIDGET TO MTLOBE MANAGER";
	QWidget *si=&w;
	if(!mWidgets.contains(si)) {
		mWidgets.push_back(si);
		ui->widgetCompressedContent->layout()->addWidget(si);
		si->setParent(ui->widgetCompressedContent);
		QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		si->setMinimumSize(0,0);
		si->setSizePolicy(sizePolicy);
		si->updateGeometry();
		si->adjustSize();

		if(!connect(si,SIGNAL(mtlobeDeleted(quint32)),mController,SLOT(onMtlobeWidgetDeleted(quint32)),OC_CONTYPE)) {
			qWarning()<<"ERROR: could not connect";
		}
	}
}

void MtlobeManagerWidget::removeWidget(QWidget &w)
{
	qDebug()<<"--- REMOVING WIDGET FROM MTLOBE MANAGER";
	if(mWidgets.contains(&w)) {
		mWidgets.removeAll(&w);
		ui->widgetCompressedContent->layout()->removeWidget(&w);
		QWidget *si=&w;
		if (nullptr != si)  {
			if(!disconnect(si,SIGNAL(mtlobeDeleted(quint32)),mController,SLOT(onMtlobeWidgetDeleted(quint32)))) {
				qWarning()<<"ERROR: could not disconnect";
			}
		}
		w.setParent(nullptr);
	}
}

void MtlobeManagerWidget::configure(MtlobeController *controller)
{

	if(nullptr!=mController) {
		if(!disconnect(mController,SIGNAL(mtlobeConfigurationChanged()),this,SLOT(mtlobeManagerChanged()))) {
			qWarning()<<"ERROR: could not disconnect";
		}
		/*
		MtlobeSet &lobes=mController->loci();
		quint32 num=lobes.size();
		for(quint32 i=0; i<num; ++i) {
			Mtlobe *lobe=lobes[i];
			if(nullptr!=lobe) {
				QWidget *si=lobe->configurationWidget();
				if(nullptr!=si) {
					removeWidget(*si);
				}
			}
		}
		*/
		mWidgets.clear();
	}
	mController=controller;
	if(nullptr!=mController) {
		if(!connect(mController,SIGNAL(mtlobeConfigurationChanged()),this,SLOT(mtlobeManagerChanged()),OC_CONTYPE)) {
			qWarning()<<"ERROR: could not connect";
		}
		MtlobeSet &lobes=mController->lobes();
		quint32 num=lobes.size();
		for(quint32 i=0; i<num; ++i) {
			Mtlobe *lobe=lobes[i];
			if(nullptr!=lobe) {
				QWidget *si=lobe->configurationWidget();
				if(nullptr!=si) {
					addWidget(*si);
				}
			}
		}
	}
}



void MtlobeManagerWidget::mtlobeManagerChanged()
{
	// Reload config the best we can
	configure(mController);
}


