#ifndef ONEOFFSPEECH_HPP
#define ONEOFFSPEECH_HPP

#include <QObject>
#include <QString>

class CreepyVoice;
class AudioStream;
class PortableID;

/*
 Convenience class to set up audio output, say a few words and then shut it down, all in one go.

 Useful when you just want to speak a truth or two without keeping up expensive audio output for long periods of time.

 Not suitable when you are going to say a lot, for example reading a full text or frequent ongoing reporting.

*/
class OneOffSpeech: public QObject
{
	Q_OBJECT
private:
	CreepyVoice *esw;
	AudioStream *as;
public:
	explicit OneOffSpeech(PortableID &id, QString words, QObject *parent=nullptr);
	virtual ~OneOffSpeech();

public slots:

	void onPeriodComplete();
};

#endif // ONEOFFSPEECH_HPP
