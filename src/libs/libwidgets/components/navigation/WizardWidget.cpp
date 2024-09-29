#include "WizardWidget.hpp"
#include "ui_WizardWidget.h"
#include "uptime/MethodGate.hpp"

WizardWidget::WizardWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::WizardWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
	// Make the wizard empty by default
	ui->stackedWidgetPages->removeWidget(ui->page);
	ui->stackedWidgetPages->removeWidget(ui->page_2);
	reset();
}

WizardWidget::~WizardWidget()
{
	OC_METHODGATE();
	delete ui;
}


void WizardWidget::registerPage(QWidget *page, bool headerEnabled, bool backEnabled, bool menuEnabled){
	OC_METHODGATE();
	auto title = page->objectName();
	mPageMap[title] = WizardPage(page, headerEnabled, backEnabled, menuEnabled);
	ui->stackedWidgetPages->addWidget(page);
	updateUI();
}



void WizardWidget::updateUI(){
	OC_METHODGATE();
	const auto cur = ui->stackedWidgetPages->currentWidget();
	if(nullptr != cur){
		const auto title{cur->objectName()};
		const auto ct{ui->stackedWidgetPages->count()};
		const auto index=ui->stackedWidgetPages->currentIndex();
		ui->labelPageTitle->setText(title);
		ui->labelPageIndex->setText(QString("%1 / %2").arg(index + 1).arg(ct));
	}
	else{
		ui->labelPageTitle->setText("None");
		ui->labelPageIndex->setText("0 / 0");
	}
}

void WizardWidget::navigate(const QString &title){
	OC_METHODGATE();
	if(mPageMap.contains(title)){
		auto &page = mPageMap[title];
		auto widget = page.widget;
		const auto lastWidget = ui->stackedWidgetPages->currentWidget();
		if(lastWidget!=widget){
			ui->stackedWidgetPages->setCurrentWidget(widget);
		}else{
			qWarning() << "Already on page" << title;
		}
	}else{
		qWarning() << "Failed to go to unknown page" << title;
	}
}


/*

void WizardWidget::navigate(QString destination){
OC_METHODGATE();
	qDebug()<<"AUTH NAVIGATING TO"<<destination;
	auto parts = destination.split(".").toList();
	QWidget *page{nullptr};
	const auto us = parts.takeFirst();
	if(us != "authenticate"){
		qDebug()<<"AUTH NOPE"<<destination;
		return;
	}
	const auto first = parts.takeFirst();
	const auto rest = parts.join(".");
	
	if(first == "login"){
		page = ui->pageLogin;
	}
	else if(first == "create"){
		page = ui->pageCreate;
	}
	else if(first == "transfer"){
		page = ui->pageTransfer;
	}
	else if(first  == "lobby"){
		page = ui->pageUnlocked;
	}
	else{
		qDebug()<<"UNHANDLED FIRST:"<<first;
		qDebug()<<"WITH REST:"<<rest;
	}
	if(nullptr != page){
		const auto lastPage = ui->stackedWidget->currentWidget();
		if(lastPage!=page){
			qDebug()<<"Navigated from"<<lastPage->objectName() <<" --> "<<page->objectName();
			ui->stackedWidget->setCurrentWidget(page);
			page->setFocus();
			//const auto &name = pageWidgetToName(page);
			//mSettings->setValue(SETTINGS_KEY_INITIAL_SCREEN, name);
		}
	}
	else{
		qWarning()<<"Unknown destination:"<<destination;
	}
}
*/




void WizardWidget::navigateRelative(int rel){
	OC_METHODGATE();
	//const QWidget *cur=ui->stackedWidgetPages->currentWidget();
	const int curIndex=ui->stackedWidgetPages->currentIndex();
	const auto ct{ui->stackedWidgetPages->count()};
	int next{0};
	if(mLooping){
		next = (curIndex + rel) % ct;
	}
	else{
		next = curIndex + rel;
		if(next < 0 || next >= curIndex){ // ui->stackedWidget->currentIndex()
			qDebug()<<"END REACHED, DONE";
			emit done();
		}
		else{
		}
	}
	navigateAbsolute(next);
}



void WizardWidget::navigateAbsolute(int target){
	ui->stackedWidgetPages->setCurrentIndex(target);
	const auto cur=ui->stackedWidgetPages->currentWidget();
	updateUI();
	if(nullptr != cur){
		const auto title{cur->objectName()};
		if(mPageMap.contains(title)){
			auto &page = mPageMap[title];
			emit currentPageChanged(page);
		}
	}
}



void WizardWidget::reset(){
	// TODO: Reset state and go to first page
	navigateAbsolute(0);
}


void WizardWidget::prev(){
	navigateRelative(-1);
}


void WizardWidget::next(){
	navigateRelative(+1);
}
