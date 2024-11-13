#include "StanzaManagerActivity.hpp"
#include "agent/Agent.hpp"
#include "agent/AgentConfig.hpp"
#include "ui_StanzaManagerActivity.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "stanza/StanzaItemDelegate.hpp"
#include "stanza/StanzaListModel.hpp"


StanzaManagerActivity::StanzaManagerActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::StanzaManagerActivity)
	, mAgent(nullptr)
	, mList(nullptr)
	, mDelegate (nullptr)
	, mConfigureHelper("StanzaManagerActivity", true, false, false, true, false)

{
	OC_METHODGATE();
	ui->setupUi(this);
}


StanzaManagerActivity::~StanzaManagerActivity()
{
	OC_METHODGATE();
	delete ui;
}


void StanzaManagerActivity::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mAgent = agent;
		updateList();
	}
}


void StanzaManagerActivity::updateList(){
	if(mList != nullptr){
		mList->invalidate();
	}
}


void StanzaManagerActivity::create(){
	push("StanzaEditorActivity");
}


void StanzaManagerActivity::done(){
	pop();
}


void StanzaManagerActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(!mAgent.isNull() ) {
		if(nullptr == ui->listViewStanzas->model()) {
			auto config = mAgent->configuration();
			if(!config.isNull()){
				const auto stanzaName = config->stanzaName();
				const auto stanzas = config->stanzas();
				if(!stanzas.isNull()){
					mList = OC_NEW StanzaListModel(stanzas);
				} else {
					qWarning()<<"WARNING: No stanzas";
				}
			}
			ui->listViewStanzas->setModel(mList);
			if(nullptr == mDelegate) {
				mDelegate = OC_NEW StanzaItemDelegate(this);
			}
			ui->listViewStanzas->setItemDelegate(mDelegate);
			if(nullptr != mDelegate) {
				if(!connect(mDelegate, &StanzaItemDelegate::startEdit, [this](const QString id) {
						qDebug() << "EDIT STARTED FOR " << id;
						emit startEdit(id);
					}
							 )) {
					qWarning() << "ERROR: Could not connect " << mDelegate->objectName();
				}
			}
			else{
				qWarning() << "ERROR: No delegate";
			}
		}
	} else {
		qWarning()<<"WARNING: No node";
	}
}


void StanzaManagerActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	qDebug() << "stanza manager pop args" << returnActivity << returnArguments;
	if("StanzaEditorActivity" == returnActivity){
		updateList();
	}
}
