#ifndef CREEPYVOICE_HPP
#define CREEPYVOICE_HPP

#include "IAudioSource.hpp"

#include "Reverb.hpp"
#include "Compressor.hpp"
#include "Tremolo.hpp"


#ifdef EXTERNAL_LIB_ESPEAK
#include "espeak/speak_lib.h"
#endif

#include <QList>
#include <QAudioFormat>


struct CreepyBuffer;
class RNG;
class PortableID;


class CreepyVoice: public QObject, public IAudioSource
{
	Q_OBJECT
private:
	int mHz;
	bool mInited;
	QList<CreepyBuffer *> mBuffersInUse;
	QList<CreepyBuffer *> mBuffersFree;
	int bac;
	CreepyBuffer *cur;
	QAudioFormat af;
	int periodCounter;
	Tremolo tremolo;
	Compressor  comp;
	ReverbEffect reverb;
	double floatSamplesOut[1000000];
	PortableID &mID;
	RNG *rng;
	const double distAmount=1.1;


public:
	explicit CreepyVoice(PortableID &id, QObject *parent=nullptr);
	virtual ~CreepyVoice();

private:

	double frandGauss();
	double frandGaussAbs();

	void freeBuffer(CreepyBuffer *buf);
	CreepyBuffer *getFreeBuffer(int numsamples);
public:
#ifdef EXTERNAL_LIB_ESPEAK
	void feed(short *wav, int numsamples, espeak_EVENT *events);
#endif
	bool isInitialized();
	void speak(QString word);
	void deinit();


	//IAudioSource interface
public:
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void generate(qint64 num, double *out)  Q_DECL_OVERRIDE;

signals:

	void periodComplete();
};


#endif // CREEPYVOICE_HPP
