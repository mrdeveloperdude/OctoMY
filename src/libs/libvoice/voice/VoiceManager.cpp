#include "VoiceManager.hpp"

#include "security/PortableID.hpp"
#include "uptime/MethodGate.hpp"



#include <QDebug>
#include <QtMath>


VoiceManager::VoiceManager(QObject *parent)
	: QObject(parent)
{
	OC_METHODGATE();
}


VoiceManager::~VoiceManager()
{
	OC_METHODGATE();
}


QSharedPointer<CreepyVoice<qreal> > VoiceManager::creepyVoice{nullptr};

QSharedPointer<CreepyVoice<qreal> > VoiceManager::getCreepyVoice(){
	OC_FUNCTIONGATE();
	if(VoiceManager::creepyVoice.isNull()){
		VoiceManager::creepyVoice = QSharedPointer<CreepyVoice<qreal> >::create();
		QAudioFormat af;
		//qDebug()<<"AF"<<af;
		VoiceManager::creepyVoice->init(af);
	}
	return VoiceManager::creepyVoice;
}


bool VoiceManager::speak(const PortableID &id, const QString text)
{
	OC_FUNCTIONGATE();
	auto cv=getCreepyVoice();
	if(cv.isNull()){
		qWarning() << "Creepy voice was NULL";
	}
	return cv.isNull()?false:cv->speak(id, text);
}


bool VoiceManager::voiceIsAvailable(){
	OC_FUNCTIONGATE();
	auto cv = getCreepyVoice();
	return cv.isNull()?false:cv->voiceIsAvailable();
}
