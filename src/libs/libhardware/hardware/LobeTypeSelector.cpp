#include "LobeTypeSelector.hpp"
#include "ui_LobeTypeSelector.h"


#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentConfig.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "lobe/LobeFactory.hpp"
#include "lobe/Lobe.hpp"
#include "hardware/models/LobeTypeModel.hpp"



LobeTypeSelector::LobeTypeSelector(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::LobeTypeSelector)
	, mLobeTypeModel(OC_NEW LobeTypeModel())
{
	OC_METHODGATE();
	ui->setupUi(this);
	updateList();
}


LobeTypeSelector::~LobeTypeSelector()
{
	OC_METHODGATE();
	delete ui;
}


void LobeTypeSelector::updateList()
{
	OC_METHODGATE();
	mLobeTypeModel->clear();
	LobeFactory factory;
	const auto available = factory.availableLobes();
	QString title("None");
	QString description("No Lobe");
	QString icon(":/icons/unknown.svg");
	for(auto const &type:available){
		auto lobe = factory.createLobe(type);
		if(!lobe.isNull()){
			title = lobe->lobeTitle();
			icon = lobe->lobeIcon();
			description = lobe->lobeDescription();
		}
		mLobeTypeModel->registerLobeType(title, description, icon);
	}
	ui->listViewLobeType->setModel(mLobeTypeModel);
}


void LobeTypeSelector::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
	reset();
}


void LobeTypeSelector::reset()
{
	OC_METHODGATE();
	
}


void LobeTypeSelector::selectLobe(){
	OC_METHODGATE();
	auto const index = ui->listViewLobeType->currentIndex();
	auto const item = mLobeTypeModel->getNickName(index);
	pop(QStringList() << item);
}
