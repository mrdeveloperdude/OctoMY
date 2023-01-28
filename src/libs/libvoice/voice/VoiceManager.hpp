#ifndef VOICE_MANAGER_HPP
#define VOICE_MANAGER_HPP

#include "audio/sources/CreepyVoice.hpp"

#include <QObject>
#include <QSharedPointer>

class PortableID;

class VoiceManager: public QObject
{
	Q_OBJECT
private:
	static QSharedPointer<CreepyVoice<qreal> > creepyVoice;
public:
	explicit VoiceManager(QObject *parent=nullptr);
	virtual ~VoiceManager() Q_DECL_OVERRIDE;

public:
	static QSharedPointer<CreepyVoice<qreal> > getCreepyVoice();
	static bool speak(PortableID &id, QString text);
	static bool voiceIsAvailable();

};


#endif
// VOICE_MANAGER_HPP
