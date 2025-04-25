#include "PlanBookWidget.hpp"
#include "ui_PlanBookWidget.h"

#include "node/Node.hpp"
#include "plan/book/PlanBookModel.hpp"
#include "plan/book/PlanBook.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


PlanBookWidget::PlanBookWidget(
	QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PlanBookWidget)
	, mConfigureHelper("PlanBookWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

PlanBookWidget::~PlanBookWidget()
{
	OC_METHODGATE();
	delete ui;
}


void PlanBookWidget::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		if(mPlanBookModel){
			mPlanBookModel->deleteLater();
			mPlanBookModel = nullptr;
		}
		mPlanBookModel = OC_NEW PlanBookModel(mNode->planBook());
		//log("SETTING UP PLAN BOOK");
		ui->widgetPlans->configure(mNode->settings());
		ui->widgetPlans->setModel(mPlanBookModel);
		connect(ui->widgetPlans, &MultiView::itemDoubleClicked, this, &PlanBookWidget::itemDoubleClicked);
		
	}
}


void PlanBookWidget::createPlan(){
	OC_METHODGATE();
	qDebug() << "Create plan(s)";
	auto planBook = mPlanBookModel->planBook();
	if(planBook){
		auto plan = QSharedPointer<Plan>::create();
		plan->setContent("BALLS");
		planBook->upsertPlan(plan);
	}
	else{
		qWarning() << "No plan book";
	}
}

void PlanBookWidget::deletePlan(){
	OC_METHODGATE();
	qDebug() << "Delete plan(s)";
	const auto selectedIndexes = ui->widgetPlans->selectionModel()->selectedIndexes();
	mPlanBookModel->deleteSelected(selectedIndexes);
}

void PlanBookWidget::duplicatePlan(){
	OC_METHODGATE();
	qDebug() << "Duplicate plan(s)";
	const auto selectedIndexes = ui->widgetPlans->selectionModel()->selectedIndexes();
	mPlanBookModel->duplicateSelected(selectedIndexes);
	
}


void PlanBookWidget::setDefaultPlan(){
	OC_METHODGATE();
	qDebug() << "Set as Default Plan";
	const auto selectedIndexes = ui->widgetPlans->selectionModel()->selectedIndexes();
	if(1 == selectedIndexes.count()){
		auto planBook = mPlanBookModel->planBook();
		if(planBook){
			auto plan = mPlanBookModel->plan(selectedIndexes.at(0));
			if(plan){
				auto dID = plan->id();
				qDebug() << "Setting default plan to " << dID;
				planBook->setDefaultPlan(dID);
			}
			else{
				qWarning() << "No plan for selected index";
			}
		}
		else{
			qWarning() << "No plan book";
		}
	}
	else{
		qWarning()<<"Only one plan can be the default";
	}
}


void PlanBookWidget::synchronizePlans(){
	OC_METHODGATE();
	qDebug() << "Synchronize plan(s)";
	auto planBook = mPlanBookModel->planBook();
	if(planBook){
		planBook->synchronize();
	}
	else{
		qWarning() << "No plan book";
	}
}


void PlanBookWidget::itemDoubleClicked(const QModelIndex &index){
	OC_METHODGATE();
	qDebug() << "itemDoubleClicked" << index;
	if(mPlanBookModel){
		auto plan = mPlanBookModel->plan(index);
		if(plan){
			emit planSelected(plan);
		}
		else{
			qWarning() << "No Plan for index";
		}
	}
	else{
		qWarning() << "No PlanBookModel";
	}
}


