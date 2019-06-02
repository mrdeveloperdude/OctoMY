#include "SpeechControlWidget.hpp"
#include "ui_SpeechControlWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "utility/time/HumanTime.hpp"

#include "security/PortableID.hpp"
#include "security/SecurityConstants.hpp"
#include "audio/OneOffSpeech.hpp"
#include "audio/CreepyVoice.hpp"
#include "utility/ui/GenericKeyEventHandler.hpp"


#include <QCryptographicHash>
#include <QDateTime>


SpeechControlWidget::SpeechControlWidget(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::SpeechControlWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->logScrollHistory->setDirection(true);

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

	const bool available=CreepyVoice::voiceIsAvailable();

	ui->widgetVoiceEnabledStatus->setLightOn(available);
	ui->pushButtonSay->setEnabled(available);
	ui->label->setText(QStringLiteral("Speech ")+QString(available?"available":"not available"));
}

SpeechControlWidget::~SpeechControlWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}



void SpeechControlWidget::appendSpeechHistory(const QString& text)
{
	OC_METHODGATE();
	ui->logScrollHistory->appendLog(text.trimmed());
}

//#include "random/RNG.hpp" RNG *rng=RNG::sourceFactory("mt");

void SpeechControlWidget::on_pushButtonSay_clicked()
{
	OC_METHODGATE();
	QString text=ui->plainTextEditSpeechText->toPlainText().trimmed();
	if(""!=text) {
		appendSpeechHistory(text);

		ui->plainTextEditSpeechText->clear();
		PortableID id;
		//	=mController->localNodeAssociate()->toPortableID();
		QCryptographicHash ch(OCTOMY_KEY_HASH);
		QByteArray ba;
		union {
			quint8 c8[8];
			quint64 i64;
		} a;
		a.i64=utility::time::currentMsecsSinceEpoch<quint64>();
		for(int i=0; i<8; ++i) {
			ba[i]=static_cast<char>(a.c8[i]);
		}
		ch.addData(ba);
		id.setID(ch.result().toHex().toUpper());
		OC_NEW OneOffSpeech(id, text);
	}
}
