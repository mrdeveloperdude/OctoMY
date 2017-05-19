#include "LocusManagerWidget.hpp"
#include "ui_LocusManagerWidget.h"


#include "utility/Standard.hpp"
#include "locus/WheeledLocusWidget.hpp"
#include "locus/Locus.hpp"

#include "locus/LocusController.hpp"


#include <QSpacerItem>


LocusManagerWidget::LocusManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LocusManagerWidget)
	, mController(nullptr)
{
	ui->setupUi(this);
}

LocusManagerWidget::~LocusManagerWidget()
{
	delete ui;
}


void LocusManagerWidget::addWidget(QWidget &w)
{
	qDebug()<<"--- ADDING WIDGET TO LOCUS MANAGER";
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

		if(!connect(si,SIGNAL(locusDeleted(quint32)),mController,SLOT(onLocusWidgetDeleted(quint32)),OC_CONTYPE)) {
			qWarning()<<"ERROR: could not connect";
		}
	}
}

void LocusManagerWidget::removeWidget(QWidget &w)
{
	qDebug()<<"--- REMOVING WIDGET FROM LOCUS MANAGER";
	if(mWidgets.contains(&w)) {
		mWidgets.removeAll(&w);
		ui->widgetCompressedContent->layout()->removeWidget(&w);
		QWidget *si=&w;
		if (nullptr != si)  {
			if(!disconnect(si,SIGNAL(locusDeleted(quint32)),mController,SLOT(onLocusWidgetDeleted(quint32)))) {
				qWarning()<<"ERROR: could not disconnect";
			}
		}
		w.setParent(nullptr);
	}
}

void LocusManagerWidget::configure(LocusController *controller)
{

	if(nullptr!=mController) {
		if(!disconnect(mController,SIGNAL(locusConfigurationChanged()),this,SLOT(locusManagerChanged()))) {
			qWarning()<<"ERROR: could not disconnect";
		}
		/*
		LocusSet &loci=mController->loci();
		quint32 num=loci.size();
		for(quint32 i=0; i<num; ++i) {
			Locus *locus=loci[i];
			if(nullptr!=locus) {
				QWidget *si=locus->configurationWidget();
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
		if(!connect(mController,SIGNAL(locusConfigurationChanged()),this,SLOT(locusManagerChanged()),OC_CONTYPE)) {
			qWarning()<<"ERROR: could not connect";
		}
		LocusSet &loci=mController->loci();
		quint32 num=loci.size();
		for(quint32 i=0; i<num; ++i) {
			Locus *locus=loci[i];
			if(nullptr!=locus) {
				QWidget *si=locus->configurationWidget();
				if(nullptr!=si) {
					addWidget(*si);
				}
			}
		}
	}
}



void LocusManagerWidget::locusManagerChanged()
{
	// Reload config the best we can
	configure(mController);
}


