#include "OneOffSpeech.hpp"

#include "CreepyVoice.hpp"
#include "AudioStream.hpp"
#include "utility/Standard.hpp"
#include "security/PortableID.hpp"



OneOffSpeech::OneOffSpeech(PortableID &id, QString words, QObject *parent)
	: QObject(parent)
	, esw(OC_NEW CreepyVoice(id,this))
	, as(OC_NEW AudioStream(*esw,this))
{
	if(!connect(esw,&CreepyVoice::periodComplete, this, &OneOffSpeech::onPeriodComplete, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	} else {
		as->init();
		esw->speak(words);
	}
}

OneOffSpeech::~OneOffSpeech()
{
}

void OneOffSpeech::onPeriodComplete()
{
	//qDebug()<<"PERIOD OVER";
	deleteLater();
}
