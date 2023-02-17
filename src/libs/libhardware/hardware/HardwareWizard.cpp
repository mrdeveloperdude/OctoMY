#include "HardwareWizard.hpp"
#include "ui_HardwareWizard.h"

#include "HardwareTemplate.hpp"
#include "hardware/serial/SerialDeviceListModel.hpp"
#include "hardware/HardwareTemplateModel.hpp"
#include "hardware/controllers/IActuatorController.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentConfig.hpp"

#include <QDebug>


HardwareWizard::HardwareWizard(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::HardwareWizard)
	, mSerialDevicesModel(OC_NEW SerialDeviceListModel(this))
	, mHardwareTemplateModel(OC_NEW HardwareTemplateModel(this))
	, mSelectedTempalte(nullptr)
	, mAgent(nullptr)
#ifdef OC_USE_FEATURE_MOTORICS
	, mWidgetMotorLobeManager(nullptr)
#endif
{
	OC_METHODGATE();
	ui->setupUi(this);
	setupMtLobeUi();
	initControllerList();
	//qDebug()<<"Setting model";
	ui->listViewSerialInterface->setModel(mSerialDevicesModel);
	ui->listViewTemplate->setModel(mHardwareTemplateModel);
	moveTo(0);
}


HardwareWizard::~HardwareWizard()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


#ifdef OC_USE_FEATURE_MOTORICS
#include "MtlobeManagerWidget.hpp"
#endif

void HardwareWizard::setupMtLobeUi()
{
	OC_METHODGATE();
#ifdef OC_USE_FEATURE_MOTORICS
	mWidgetMotorLobeManager = new MtlobeManagerWidget(ui->pageMotorLobes);
	mWidgetMotorLobeManager->setObjectName(QStringLiteral("widgetMotorLobeManager"));
	QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(widgetMotorLobeManager->sizePolicy().hasHeightForWidth());
	mWidgetMotorLobeManager->setSizePolicy(sizePolicy1);
	ui->verticalLayout_4->addWidget(mWidgetMotorLobeManager);

	mWidgetMotorLobeManager->configure(nullptr);
#endif
}



template<typename T>
static T mod(T a, T b)
{
	return (a%b+b)%b;
}


void HardwareWizard::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent=agent;
	reset();
}

void HardwareWizard::reset()
{
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		QSharedPointer<AgentConfigStore> configStore=mAgent->configurationStore();
		if(!configStore.isNull()) {
			QSharedPointer<AgentConfig> config=configStore->agentConfig();
			if(!config.isNull()) {
				QString name=config->controllerName();
				int ctlIndex=controllerIndexByName(name);
				ui->listWidgetController->setCurrentRow(ctlIndex);
			} else {
				qWarning()<<"ERROR: config was null while reseting hardware wizard";
			}
		} else {
			qWarning()<<"ERROR: configStore was null while reseting hardware wizard";
		}
	} else {
		qWarning()<<"ERROR: Agent was null while reseting hardware wizard";
	}
	moveTo(0);
}


static void selectFirstIfNoneSelected(QListView *list)
{
	OC_FUNCTIONGATE();
	list->setFocus();
	if(!list->selectionModel()->hasSelection()) {
		list->selectionModel()->setCurrentIndex(list->model()->index(0,0),QItemSelectionModel::Select);
	}
}

void HardwareWizard::initControllerList()
{
	OC_METHODGATE();
	controllerStanzas.clear();
	ui->listWidgetController->clear();
	controllerStanzas << ControllerStanza("none", "None", ":/icons/unknown.svg");
	controllerStanzas << ControllerStanza("ardumy", "ArduMY™ native OctoMY™ <--> Arduino© binding", ":/icons/ardumy.svg");
	controllerStanzas << ControllerStanza("servotor32", "ArcBotics Servotor32 v2.1", ":/icons/arcbotics_logo.svg");

	for(auto stanza:controllerStanzas) {
		QIcon icon;
		icon.addFile(stanza.iconURL, QSize(), QIcon::Normal, QIcon::Off);
		QListWidgetItem *temp = OC_NEW QListWidgetItem(ui->listWidgetController);
		temp->setIcon(icon);
		temp->setText(stanza.fullName);
		temp->setToolTip(stanza.nickName);
	}
}



void HardwareWizard::initMtlobeList()
{
	OC_METHODGATE();
	mMtlobeStanzas.clear();
	mMtlobeStanzas << MtlobeStanza("none", "None", ":/icons/unknown.svg");
	mMtlobeStanzas << MtlobeStanza("wheeled", "Wheeled motion", ":/icons/wheels.svg");
	mMtlobeStanzas << MtlobeStanza("tracked", "Tracked motion", ":/icons/threads.svg");
	mMtlobeStanzas << MtlobeStanza("legged", "Legged motion", ":/icons/hexapod.svg");

	for(auto stanza:mMtlobeStanzas) {
		QIcon icon;
		icon.addFile(stanza.iconURL, QSize(), QIcon::Normal, QIcon::Off);
		/*
		QListWidgetItem *temp = OC_NEW QListWidgetItem(ui->comboBoxAddMtlobe);
		temp->setIcon(icon);
		temp->setText(stanza.fullName);
		temp->setToolTip(stanza.nickName);
		*/
	}
}


int HardwareWizard::controllerIndexByName(QString name)
{
	OC_METHODGATE();
	int index=0;
	for(auto stanza:controllerStanzas) {
		if(stanza.nickName==name) {
			return index;
		}
		index++;
	}
	return 0;
}

QString HardwareWizard::selectedControllerName()
{
	OC_METHODGATE();
	const int index=ui->listWidgetController->currentIndex().row();
	if(index>=0 && index< controllerStanzas.size()) {
		ControllerStanza stanza=controllerStanzas[index];
		return stanza.nickName;
	}
	return "none";
}

void HardwareWizard::moveTo(int next)
{
	OC_METHODGATE();
	switch(next) {
	// Controller
	case(0): {
		selectFirstIfNoneSelected(ui->listWidgetController);
	}
	break;
	// Controller config
	case(1): {
		QSharedPointer<IActuatorController> ctl;
		if(!mAgent.isNull()) {
			mAgent->reloadController();
			ctl=mAgent->actuatorController();
		}
		QWidget *configurationWidget=nullptr;
		if(!ctl.isNull()) {
			QWidget *temp=ctl->configurationWidget();
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
	// Loci management
	case(2): {

	}
	break;

	// Interface
	case(3): {
		// NOTE: We end here because the rest of the pages are work in progress
		emit done();
		selectFirstIfNoneSelected(ui->listViewSerialInterface);
	}
	break;

	// Serial device
	case(4): {
		selectFirstIfNoneSelected(ui->listViewSerialInterface);
	}
	break;
	// Template
	case(5): {
		selectFirstIfNoneSelected(ui->listViewTemplate);
	}
	break;
	// Serial settings
	case(6): {

	}
	break;
	// Pose Mapping
	case(7): {
	}
	break;
	case(8): {

		emit done();
		//selectFirstIfNoneSelected(ui->listWidgetProtocol);
	}
	break;
	}
	ui->stackedWidget->setCurrentIndex( next );
}


void HardwareWizard::save()
{
	OC_METHODGATE();
	QSharedPointer<IActuatorController> ctl;
	if(!mAgent.isNull()) {
		QSharedPointer<AgentConfigStore> configStore=mAgent->configurationStore();
		QSharedPointer<AgentConfig> config=configStore->agentConfig();
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
			qDebug().noquote().nospace()<<" S A V E called with cur= "<<cur;

			// Controller
			if(cur==ui->pageController) {
				QString controllerName=selectedControllerName();
				qDebug().noquote().nospace()<< "Saving controller name '"<<controllerName<<"' in Agent Config Store";
				config->setControllerName(controllerName);
			}

			// Controller Configuration
			else if (cur == ui->pageControllerConfiguration) {
				ctl=mAgent->actuatorController();
				if(!ctl.isNull()) {
					qDebug().noquote().nospace()<< "Saving controller data";
					config->setControllerConfig(ctl->configuration());
				} else {
					qWarning()<<"WARNING: Saving empty controller config in hardware wizard";
					QVariantMap empty;
					config->setControllerConfig(empty);
				}
			} else {
				qDebug()<<"WARNING: S A V E called with unknwon cur";
			}

		} else {
			qWarning()<<"ERROR: Could not find config while saving";
		}
	} else {
		qWarning()<<"ERROR: No Agent while saving";
	}
}

void HardwareWizard::loadFromTemplate()
{
	OC_METHODGATE();
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
	OC_METHODGATE();
	save();
	const QWidget *cur=ui->stackedWidget->currentWidget();
	int curIndex=ui->stackedWidget->currentIndex();
	if(cur == ui->pageController) {
		if ("none" == selectedControllerName()) {
			// HACK: Skip the controller config page
			curIndex++;
		}
	}
	const int next=mod(curIndex+1, ui->stackedWidget->count());
	moveTo(next);
}

void HardwareWizard::on_pushButtonBack_clicked()
{
	OC_METHODGATE();
	const int next=mod(ui->stackedWidget->currentIndex()-1, ui->stackedWidget->count());
	moveTo(next);
}

void HardwareWizard::on_listWidgetController_doubleClicked(const QModelIndex &index)
{
	OC_METHODGATE();
	Q_UNUSED(index);
	on_pushButtonOnward_clicked();
}

void HardwareWizard::on_comboBoxAddMtlobe_currentIndexChanged(int index)
{
	OC_METHODGATE();
	switch(index) {
	default:
	case(0): {
		return;
	}
	//break;
#ifdef OC_USE_FEATURE_MOTORICS
	// Wheeled
	case(1): {
		mMtlobeController.addLobe("wheeled");
	}
	break;
	// Tracked
	case(2): {
		mMtlobeController.addLobe("tracked");
	}
	break;
	// Legged
	case(3): {
		mMtlobeController.addLobe("legged");
	}
	break;
	// Hovering
	case(4): {
		mMtlobeController.addLobe("hovering");
	}
	break;
#endif
	}
#ifdef OC_USE_FEATURE_MOTORICS
	ui->widgetMotorLobeManager->configure(&mMtlobeController);
#endif
	ui->comboBoxAddMtlobe->setCurrentIndex(0);
}
