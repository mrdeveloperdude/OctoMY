#include "SpeechControlWidget.hpp"
#include "ui_SpeechControlWidget.h"

#include "log/LogStorage.hpp"
#include "node/Node.hpp"
#include "security/PortableID.hpp"
#include "security/SecurityConstants.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/time/HumanTime.hpp"
#include "utility/ui/GenericKeyEventHandler.hpp"
#include "voice/VoiceManager.hpp"


#include <QCryptographicHash>
#include <QDateTime>
#include "uptime/SharedPointerWrapper.hpp"


SpeechControlWidget::SpeechControlWidget(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::SpeechControlWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->logScrollHistory->setLogDirection(true);

	GenericKeyEventHandler *gkh=OC_NEW GenericKeyEventHandler(ui->plainTextEditSpeechText);
	gkh->setEventProcessor([=](QObject *o, QKeyEvent *keyEvent) {
		Q_UNUSED(o);
		auto t=keyEvent->type();
		if(t==QEvent::KeyPress || t==QEvent::KeyRelease) {
			keyEvent->accept();
			if (nullptr!=keyEvent && keyEvent->key() == Qt::Key_Return) {
				if(keyEvent->modifiers() != Qt::ControlModifier) {
					// Return without modifier pressed
					if (t == QEvent::KeyRelease) {
						ui->pushButtonSay->click();
						// This event has been handled
						return true;
					}
				}
			}
		}
		return false;
	});
	setSpeechAvailable(VoiceManager::voiceIsAvailable());
}

SpeechControlWidget::~SpeechControlWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void SpeechControlWidget::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	mNode = node;
}


void SpeechControlWidget::setSpeechAvailable(const bool available){
	ui->widgetVoiceEnabledStatus->setLightColor(available?LightWidget::sDefaultOKColor:LightWidget::sDefaultErrorColor);
	ui->widgetVoiceEnabledStatus->setLightOn(true);
	ui->pushButtonSay->setEnabled(available);
	ui->label->setText(QStringLiteral("Speech ")+QString(available?"available":"NOT available"));
}


void SpeechControlWidget::appendSpeechHistory(const QString& text)
{
	OC_METHODGATE();
	if(mNode){
		mNode->log(text);
	}
}

//#include "random/RNG.hpp" RNG *rng=RNG::sourceFactory("mt");

static PortableID randomID(){
	PortableID id;
	//	=mController->localNodeAssociate()->toPortableID();
	QCryptographicHash ch(OCTOMY_KEY_HASH);
	QByteArray ba(8, 0x00);
	union {
		quint8 c8[8];
		quint64 i64;
	} a;
	a.i64=utility::time::currentMsecsSinceEpoch<quint64>();
	for(int i=0; i<ba.size(); ++i) {
		ba[i]=static_cast<char>(a.c8[i]);
	}
	ch.addData(ba);
	id.setID(ch.result().toHex().toUpper());
	return id;
}

void SpeechControlWidget::on_pushButtonSay_clicked()
{
	OC_METHODGATE();
	QString text=ui->plainTextEditSpeechText->toPlainText().trimmed();
	if(""!=text) {
		ui->plainTextEditSpeechText->clear();
		PortableID id=randomID();
		auto ret=VoiceManager::speak(id, text);
		appendSpeechHistory(text+QString(ret?" OK":" FAILED"));		
	}
}
