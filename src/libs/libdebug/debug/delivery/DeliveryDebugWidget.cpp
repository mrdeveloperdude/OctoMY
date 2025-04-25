#include "DeliveryDebugWidget.hpp"
#include "ui_DeliveryDebugWidget.h"

#include "node/Node.hpp"
#include "security/Key.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

DeliveryDebugWidget::DeliveryDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeliveryDebugWidget)
	, mConfigureHelper("DeliveryDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

DeliveryDebugWidget::~DeliveryDebugWidget()
{
	OC_METHODGATE();
	delete ui;
	auto pal = ui->pushButtonScrape->palette();
	pal.setColor(QPalette::Button, Qt::darkRed);
	pal.setColor(QPalette::Text, Qt::white);
	ui->pushButtonScrape->setPalette(pal);
}


void DeliveryDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = node;
		ui->widgetBirthCertificate->configure(nullptr, true, true);
		mBirthControl.configure(mNode);
		updateUI();
		if(nullptr != ui->tryToggleBirth) {
			if(!mNode.isNull()) {
				ui->tryToggleBirth->configure("Birth", "Delivering", "Unbirth", "Killing");
				if(!QObject::connect(ui->tryToggleBirth, &TryToggle::stateChanged, mNode.data(), [=](const TryToggleState last, const TryToggleState current) {
						Q_UNUSED(last);
						if(transient(current)) {
							positive(current)?mBirthControl.birth():mBirthControl.unBirth();
						}
						updateUI();
					}, OC_CONTYPE_NON_UNIQUE)) {
				}
			} else {
				qWarning()<<"WARNING: Could not switch birth, no node";
			}
		} else {
			qWarning()<<"WARNING: No try birth try toggle";
		}
	}
}


void DeliveryDebugWidget::updateUI(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->widgetBirthCertificate->setPortableID(PortableID());
		if(!mNode.isNull()) {
			auto identity = mNode->nodeIdentity();
			if(!identity.isNull()) {
				auto pid = identity->toPortableID();
				ui->widgetBirthCertificate->setPortableID(pid);
				ui->tryToggleBirth->setState(pid.id().isEmpty()?TryToggleState::OFF:TryToggleState::ON, false);
			}
		}
		auto assessment = mBirthControl.statusAssesment();
		const auto ok = assessment.isOK();
		ui->widgetLightAllOK->setLightOn(true);
		ui->widgetLightAllOK->setLightColor(ok?LightWidget::sDefaultOKColor:LightWidget::sDefaultErrorColor);
		ui->labelHasOKStatus->setText(ok?QString("All is good"):QString("Midwife says\n:").arg(assessment.toString()));
		ui->widgetLightHasLocalKey->setLightOn(assessment.hasLocalKey);
		
		ui->widgetLightHasLocalKeyID->setLightOn(assessment.hasLocalKeyID);
		ui->labelHasLocalKeyID->setText(assessment.hasLocalKeyID?assessment.localKey->id():QString("Key has no ID"));
		
		ui->widgetLightHasLocalKeyPublic->setLightOn(assessment.hasLocalKeyPublic);
		ui->labelHasLocalKeyPublic->setText(assessment.hasLocalKeyPublic?QString("Public %1 bytes").arg(assessment.localKey->pubKey().size()):QString("No public part"));
		
		ui->widgetLightHasLocalKeyPrivate->setLightOn(assessment.hasLocalKeyPrivate);
		ui->labelHasLocalKeyPrivate->setText(assessment.hasLocalKeyPrivate?QString("Private %1 bytes").arg(assessment.localKey->key().size()):QString("No private part"));
		
		ui->widgetLightHasNodeIdentity->setLightOn(assessment.hasNodeIdentity);
		ui->widgetLightHasNodeIdentityName->setLightOn(assessment.hasNodeIdentityName);
		ui->widgetLightHasNodeIdentityGender->setLightOn(assessment.hasNodeIdentityGender);
		
		ui->widgetLightHasNodeIdentityID->setLightOn(assessment.hasNodeIdentityID);
		ui->labelHasNodeIdentityID->setText(assessment.hasNodeIdentityID?assessment.nodeIdentity->id():QString("Identity has no ID"));
		
		
		ui->widgetLightHasNodeIdentityRole->setLightOn(assessment.hasNodeIdentityRole);
		ui->labelHasNodeIdentityRole->setText(QString("Has role %1").arg(assessment.hasNodeIdentityRole?nodeRoleToString(assessment.nodeIdentity->role()):"none")); 
		
		ui->widgetLightHasNodeIdentityType->setLightOn(assessment.hasNodeIdentityType);
		ui->labelHasNodeIdentityType->setText(QString("Has type %1").arg(assessment.hasNodeIdentityType?nodeTypeToString(assessment.nodeIdentity->type()):"none")); 
		
		ui->widgetLightHasNodeIdentityIDMatchesLocalKeyID->setLightOn(assessment.hasNodeIdentityIDMatchesLocalKeyID);
	}
}


void DeliveryDebugWidget::scrapeWoom(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			mNode->scrape();
			//mNode->unbirth();
		} else {
			qWarning()<<"WARNING: Could not scrape, no node";
		}
		updateUI();
	}
}


void DeliveryDebugWidget::reload(){
	OC_METHODGATE();
	updateUI();
}
