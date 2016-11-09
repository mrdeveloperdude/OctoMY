#include "SpeechControlWidget.hpp"
#include "ui_SpeechControlWidget.h"

#include "../libutil/utility/Standard.hpp"

#include "security/PortableID.hpp"
#include "security/SecurityConstants.hpp"
#include "audio/OneOffSpeech.hpp"
#include "basic/GenericKeyEventHandler.hpp"


#include <QCryptographicHash>
#include <QDateTime>


SpeechControlWidget::SpeechControlWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SpeechControlWidget)
{
	ui->setupUi(this);
	ui->logScrollHistory->setDirection(true);

	GenericKeyEventHandler *gkh=new GenericKeyEventHandler(ui->plainTextEditSpeechText);
	gkh->setEventProcessor([=](QObject *o, QKeyEvent *keyEvent) {
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
}

SpeechControlWidget::~SpeechControlWidget()
{
	delete ui;
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
		a.i64=QDateTime::currentMSecsSinceEpoch();
		for(int i=0; i<8; ++i) {
			ba[i]=a.c8[i];
		}
		ch.addData(ba);
		id.setID(ch.result().toHex().toUpper());
		new OneOffSpeech(id, text);
	}
}
