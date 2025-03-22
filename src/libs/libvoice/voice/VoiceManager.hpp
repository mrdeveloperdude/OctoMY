#ifndef VOICE_MANAGER_HPP
#define VOICE_MANAGER_HPP

#include "audio/sources/CreepyVoice.hpp"

#include <QObject>
#include "uptime/SharedPointerWrapper.hpp"

class PortableID;

class VoiceManager: public QObject
{
	Q_OBJECT
private:
	static QSharedPointer<CreepyVoice<qreal> > creepyVoice;
public:
	explicit VoiceManager(QObject *parent=nullptr);
	virtual ~VoiceManager() override;

public:
	static QSharedPointer<CreepyVoice<qreal> > getCreepyVoice();
	static bool speak(const PortableID &id, const QString text);
	static bool voiceIsAvailable();

};


#endif
// VOICE_MANAGER_HPP
