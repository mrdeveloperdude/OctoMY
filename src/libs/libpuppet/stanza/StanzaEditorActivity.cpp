#include "StanzaEditorActivity.hpp"
#include "Stanza.hpp"
#include "agent/Agent.hpp"
#include "agent/AgentConfig.hpp"
#include "lobe/Lobe.hpp"
#include "lobe/LobeFactory.hpp"
#include "stanza/StanzaBook.hpp"
#include "ui_StanzaEditorActivity.h"
#include "uptime/MethodGate.hpp"

StanzaEditorActivity::StanzaEditorActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::StanzaEditorActivity)
	, mStanza(QSharedPointer<Stanza>::create())
{
	OC_METHODGATE();
	ui->setupUi(this);
}


StanzaEditorActivity::~StanzaEditorActivity()
{
	OC_METHODGATE();
	delete ui;
}


void StanzaEditorActivity::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
}


void StanzaEditorActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	mStanza = nullptr;
	if(arguments.size() >=1){
		const auto name = arguments[0].trimmed();
		if(name.size()>0){
			ui->lineEditStanzaTitle->setText(name);
			ui->lineEditStanzaTitle->setFocus();
			ui->lineEditStanzaTitle->selectAll();
			auto config = mAgent->configuration();
			if(!config.isNull()){
				auto stanzas = config->stanzas();
				if(!stanzas.isNull()){
					mStanza = stanzas->stanzaByID(name);
				}
			}
		}
	}
	if(mStanza.isNull()){
		mStanza = QSharedPointer<Stanza>::create();
	}
	ui->widgetPoseMapping->configure(mStanza->mapping());
}


void StanzaEditorActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	qDebug() << "stanza editor pop args" << returnActivity << returnArguments;
	if("LobeTypeSelector" == returnActivity){
		if(returnArguments.size() >=1){
			const auto type = returnArguments[0];
			LobeFactory factory;
			auto lobe = factory.createLobe(type);
			if(!lobe.isNull()){
				mStanza->setLobe(lobe);
				ui->pushButtonLobe->setIcon(QPixmap(lobe->lobeIcon()));
				ui->pushButtonLobe->setText(lobe->lobeTitle());
				ui->pushButtonLobe->setToolTip(lobe->lobeDescription());
				ui->widgetPoseMapping->configure(mStanza->mapping());
			}
		}
	}
}


void StanzaEditorActivity::selectLobe(){
	OC_METHODGATE();
	auto lobe = mStanza->lobe();
	push("LobeTypeSelector", QStringList()<<(lobe.isNull()?"":lobe->lobeTitle()));
}


void StanzaEditorActivity::save(){
	OC_METHODGATE();
	if(!mStanza.isNull()){
		mStanza->setID(ui->lineEditStanzaTitle->text().trimmed());
		auto config = mAgent->configuration();
		if(!config.isNull()){
			auto stanzas = config->stanzas();
			if(!stanzas.isNull()){
				stanzas->upsertStanza(mStanza);
			}
		}
	}
	pop(QStringList() << "true");
}


void StanzaEditorActivity::cancel(){
	OC_METHODGATE();
	pop(QStringList() << "false");
}
