#include "HardwareWizard.hpp"
#include "ui_HardwareWizard.h"

#include "HardwareTemplate.hpp"
#include "models/SerialDeviceListModel.hpp"
#include "models/HardwareTemplateModel.hpp"
#include "hw/actuators/IActuatorController.hpp"
#include "../libutil/utility/Utility.hpp"
#include <QDebug>


HardwareWizard::HardwareWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HardwareWizard)
	, mSerialDevicesModel(new SerialDeviceListModel(this))
	, mHardwareTemplateModel(new HardwareTemplateModel(this))
	, mController(nullptr)
	, mSelectedTempalte(nullptr)
	, mConfigStore(nullptr)

{
	ui->setupUi(this);
	//qDebug()<<"Setting model";
	ui->listViewSerialInterface->setModel(mSerialDevicesModel);
	ui->listViewTemplate->setModel(mHardwareTemplateModel);
	ui->widgetLociManager->configure(0);
	reset();
}

HardwareWizard::~HardwareWizard()
{
	delete ui;
}


template<typename T>
static long mod(T a, T b)
{
	return (a%b+b)%b;
}


void HardwareWizard::configure(AgentConfigStore &configStore)
{
	mConfigStore=&configStore;
}

void HardwareWizard::reset()
{
	//We skip the template screen for now, because frankly it is not useful until this menu starts overflowing with alternatives.
	//moveTo(0);
	moveTo(0);
}


static void selectFirstIfNoneSelected(QListView *list)
{
	list->setFocus();
	if(!list->selectionModel()->hasSelection()) {
		list->selectionModel()->setCurrentIndex(list->model()->index(0,0),QItemSelectionModel::Select);
	}
}


QString HardwareWizard::selectedControllerName()
{
	const quint32 index=ui->listWidgetController->currentIndex().row();
	QString controllerName;
	switch (index) {
	case(0): {
		controllerName="servotor32";
	}
	break;
	case(1): {
		controllerName="ardumy";
	}
	break;
	default:
		break;
	}
	return controllerName;
}

void HardwareWizard::moveTo(int next)
{
	switch(next) {
	// Controller
	case(0): {
		selectFirstIfNoneSelected(ui->listWidgetController);
	}
	break;
	// Controller config
	case(1): {
		QString controllerName=selectedControllerName();
		qDebug()<<"CREATING '"<<controllerName<<"' CONTROLLER";
		IActuatorController *ctl=mControllerFactory.controllerFactory(controllerName);
		if(nullptr!=ctl) {
			qDebug()<<"CREATING CONTROLLER OK";
			if(nullptr!=mController) {
				qDebug()<<"DELETING OLD CONTROLLER";
				delete mController;
			}
			mController=ctl;
			ctl=nullptr;
			//selectFirstIfNoneSelected(ui->listWidgetInterfaceType);
		}
		QWidget *configurationWidget=nullptr;
		if(nullptr!=mController) {
			QWidget *temp=mController->configurationWidget();
			if(nullptr!=temp) {
				qDebug()<<"Configuration UI found";
				configurationWidget=temp;
			} else {
				qDebug()<<"Configuration UI not found";
			}
		}
		QWidget *old=ui->scrollAreaControllerConfiguration->takeWidget();
		if(old!=configurationWidget) {
			if(nullptr!=old) {
				old->setParent(nullptr);
				old=nullptr;
			}
			ui->scrollAreaControllerConfiguration->setWidget(nullptr);
			if(nullptr!=configurationWidget) {
				ui->scrollAreaControllerConfiguration->setWidget(configurationWidget);
			} else {
				ui->scrollAreaControllerConfiguration->setWidget(ui->scrollAreaWidgetContents);
			}
			ui->scrollAreaControllerConfiguration->update();
		}
	}
	break;
	// Interface
	case(2): {
		// NOTE: We end here because the rest of the pages are work in progress
		emit done();
		selectFirstIfNoneSelected(ui->listViewSerialInterface);
	}
	break;

	// Serial device
	case(3): {
		selectFirstIfNoneSelected(ui->listViewSerialInterface);
	}
	break;
	// Template
	case(4): {
		selectFirstIfNoneSelected(ui->listViewTemplate);
	}
	break;
	// Serial settings
	case(5): {

	}
	break;
	// Pose Mapping
	case(6): {
	}
	break;
	case(7): {

		emit done();
		//selectFirstIfNoneSelected(ui->listWidgetProtocol);
	}
	break;
	}
	ui->stackedWidget->setCurrentIndex( next );
}


void HardwareWizard::save()
{
	if(nullptr!=mConfigStore) {
		QSharedPointer<AgentConfig> config=mConfigStore->agentConfig();
		if(!config.isNull()) {

			/*
				const int curTemplate=ui->listViewTemplate->currentIndex().row();
				qDebug()<<"Found template: "<<curTemplate;
				QVector<HardwareTemplate *> templates=HardwareTemplate::templates();
				if(curTemplate<0 || curTemplate>=templates.size()) {
					qWarning()<<"ERROR: Could not find selected template";
					return;
				}
				mSelectedTempalte=templates[curTemplate];
				loadFromTemplate();
			*/
			const QWidget *cur=ui->stackedWidget->currentWidget();
			qDebug()<<" S A V E calledf with cur= "<<cur;

			// Controller
			if(cur==ui->pageController) {
				QString controllerName=selectedControllerName();
				qDebug()<< "Saving controller name '"<<controllerName<<"' in Agent Cnofig Store";
				config->setControllerName(controllerName);
			}

			// Controller Configuration
			else if (cur == ui->pageControllerConfiguration) {
				if(nullptr!=mController) {

					qDebug()<< "Saving controller data";
					config->setControllerConfig(mController->confiruation());

				} else {
					qWarning()<<"ERROR: Could not find controller while saving controller config in hardware wizard";
				}
			} else {
				qDebug()<<" S A V E calledf with unknwon cur";
			}

		} else {
			qWarning()<<"ERROR: Could not find config while saving";
		}
	} else {
		qWarning()<<"ERROR: Could not find config store while saving";
	}

}

void HardwareWizard::loadFromTemplate()
{
	// TODO: Fix this
	/*
	if(nullptr!=mSelectedTempalte) {
		if(!mPoseMapping.isNull()) {
			mPoseMapping->set(mSelectedTempalte->poseMapping());
			ui->widgetPoseMapping->configure(*mPoseMapping);
		} else {
			qWarning()<<"ERROR: No pose mapping. did you forget to configure HardwareWizard?";
		}
	}
	*/

}

void HardwareWizard::on_pushButtonOnward_clicked()
{
	save();
	const int cur=ui->stackedWidget->currentIndex();
	const int next=mod(cur+1, ui->stackedWidget->count());
	moveTo(next);
}

void HardwareWizard::on_pushButtonBack_clicked()
{
	const int next=mod(ui->stackedWidget->currentIndex()-1, ui->stackedWidget->count());
	moveTo(next);
}

void HardwareWizard::on_listWidgetController_doubleClicked(const QModelIndex &index)
{
	on_pushButtonOnward_clicked();
}

void HardwareWizard::on_comboBoxAddLocus_currentIndexChanged(int index)
{
	switch(index) {
	default:
	case(0): {
		return;
	}
	break;
	case(1): {
		auto v=ui->widgetLociManager->children().size()+1;

		//auto v=ui->spinBoxActuatorCount->value()+1;
//		ui->spinBoxActuatorCount->setValue(v);
		ui->widgetLociManager->configure(v);

	}
	break;
	case(2): {
	} break;

	case(3): {
	} break;
	}
	ui->comboBoxAddLocus->setCurrentIndex(0);
}
