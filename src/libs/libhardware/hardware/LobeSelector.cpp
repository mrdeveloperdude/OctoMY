#include "LobeSelector.hpp"
#include "hardware/controllers/IActuatorController.hpp"
#include "ui_LobeSelector.h"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentConfig.hpp"


#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QListView>

LobeSelector::LobeSelector(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LobeSelector)
#ifdef OC_USE_FEATURE_MOTORICS
	, mWidgetMotorLobeManager(nullptr)
#endif

{
	OC_METHODGATE();
	ui->setupUi(this);
	setupMtLobeUi();
}

LobeSelector::~LobeSelector()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}



void LobeSelector::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
	reset();
}


void LobeSelector::reset()
{
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		auto configStore = mAgent->configurationStore();
		if(!configStore.isNull()) {
			auto config = configStore->agentConfig();
			if(!config.isNull()) {
				auto name = config->controllerName();
				//auto ctlIndex = controllerIndexByName(name);
				//ui->listWidgetController->setCurrentRow(ctlIndex);
			} else {
				qWarning()<<"ERROR: config was null while reseting hardware wizard";
			}
		} else {
			qWarning()<<"ERROR: configStore was null while reseting hardware wizard";
		}
	} else {
		qWarning()<<"ERROR: Agent was null while reseting hardware wizard";
	}
}


#ifdef OC_USE_FEATURE_MOTORICS
#include "MtlobeManagerWidget.hpp"
#endif

void LobeSelector::setupMtLobeUi()
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



void LobeSelector::initMtlobeList()
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


void LobeSelector::save()
{
	OC_METHODGATE();
	/*
	QSharedPointer<IActuatorController> ctl;
	if(!mAgent.isNull()) {
		QSharedPointer<AgentConfigStore> configStore=mAgent->configurationStore();
		QSharedPointer<AgentConfig> config=configStore->agentConfig();
		if(!config.isNull()) {

			//	const int curTemplate=ui->listViewTemplate->currentIndex().row();
			//	qDebug()<<"Found template: "<<curTemplate;
			//	QVector<HardwareTemplate *> templates=HardwareTemplate::templates();
			//	if(curTemplate<0 || curTemplate>=templates.size()) {
			//		qWarning()<<"ERROR: Could not find selected template";
			//		return;
			//	}
			//	mSelectedTempalte=templates[curTemplate];
			//	loadFromTemplate();
			
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
	*/
}


static void selectFirstIfNoneSelected(QListView *list)
{
	OC_FUNCTIONGATE();
	list->setFocus();
	if(!list->selectionModel()->hasSelection()) {
		list->selectionModel()->setCurrentIndex(list->model()->index(0,0), QItemSelectionModel::Select);
	}
}


// TOD: Not in use
void LobeSelector::select(){
	
	
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
	// TODO: look at integrating this here or someplace else
	/*
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
	*/
}

void LobeSelector::on_comboBoxAddMtlobe_currentIndexChanged(int index)
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
