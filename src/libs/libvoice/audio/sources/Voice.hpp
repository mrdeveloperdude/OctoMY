#ifndef VOICE_HPP
#define VOICE_HPP

#include "uptime/MethodGate.hpp"
#include "audio/IAudio.hpp"
#include "audio/utils/Buffer.hpp"
#include "random/RNG.hpp"
#include "security/PortableID.hpp"

#ifdef OC_USE_LIB_EXT_ESPEAK
#include <espeak-ng/speak_lib.h>
//#include "espeak/speak_lib.h"

#endif

#include <QAudioFormat>
#include <QtGlobal>
#include <QDebug>

template <typename T, int OUTPUT_CHANNELS = 2>
class Voice: public IAudioSource<T, OUTPUT_CHANNELS> {
private:
	int mHz;
	bool mInited{false};
	QAudioFormat af;
	QList<Buffer<short> *> mBuffersInUse;
	QList<Buffer<short> *> mBuffersFree;
	Buffer<short> *cur{nullptr};
	const size_t bufSize{1024};
	size_t bac{0};
	size_t periodCounter{0};
	RNG *rng{nullptr};
	
	
public:
	explicit Voice(int hz=100);
	virtual ~Voice();
public:
	
	bool speak(const PortableID &id, const QString &text) const ;
	bool voiceIsAvailable();
	
protected:
	void feed(short *wav, int numsamples);
#ifdef OC_USE_LIB_EXT_ESPEAK
	static int synthCallback(short *wav, int numsamples, espeak_EVENT *events);
#endif
	
private:
	void deinit();
	bool isInitialized() const;
	void freeBuffer(Buffer<short> *buf);
	Buffer<short> *getFreeBuffer(size_t numsamples);
	T generate();
	
	// IAudioSource interface
public:
	void init(QAudioFormat f) override;
	void generate(qint64 num, T *out) override;
	
};


////////////////////////



template <typename T, int OUTPUT_CHANNELS>
Voice<T, OUTPUT_CHANNELS>::Voice(int hz)
	: mHz(hz)
	, rng(RNG::sourceFactory("mt"))
{
	OC_METHODGATE();
}

template <typename T, int OUTPUT_CHANNELS>
Voice<T, OUTPUT_CHANNELS>::~Voice()
{
	OC_METHODGATE();
	cur=nullptr;
	for(auto *buffer:mBuffersFree) {
		delete buffer;
		buffer=nullptr;
	}
	mBuffersFree.clear();
	for(auto *buffer:mBuffersInUse) {
		delete buffer;
		buffer=nullptr;
	}
	mBuffersInUse.clear();
}



template <typename T, int OUTPUT_CHANNELS>
bool Voice<T, OUTPUT_CHANNELS>::isInitialized() const
{
	OC_METHODGATE();
	return mInited;
}



#ifdef OC_USE_LIB_EXT_ESPEAK
///////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, int OUTPUT_CHANNELS>
int Voice<T, OUTPUT_CHANNELS>::synthCallback(short *wav, int numsamples, espeak_EVENT *events)
{
	if (nullptr != wav) {
		Voice<T, OUTPUT_CHANNELS> *es = static_cast<Voice<T, OUTPUT_CHANNELS> *>(events->user_data);
		if(nullptr!=es) {
			es->feed(wav, numsamples);
		} else {
			qWarning()<<"ERROR: no espeak instance in synth callback";
		}
	}
	return 0;
}


static QString espeakErrorToString(const espeak_ERROR err){
	switch(err){
#define espeakErrorToStringStanza(A) case(A):return QStringLiteral(#A); break
	espeakErrorToStringStanza(EE_OK);
	espeakErrorToStringStanza(EE_INTERNAL_ERROR);
	espeakErrorToStringStanza(EE_BUFFER_FULL);
	espeakErrorToStringStanza(EE_NOT_FOUND);
#undef espeakErrorToStringStanza
	};
	return "UNKNOWN";
}


static QString espeakVoiceToString(const espeak_VOICE *voice){
	static char genders[4] = {' ','M','F',' '};
	if(nullptr!=voice){
		return QString("espeak_VOICE{name=%1, age=%2, gender=%3, variant=%4, languages=%5, identifier=%6}").arg(voice->name).arg(voice->age).arg(genders[voice->gender]).arg(voice->variant).arg(voice->languages).arg(voice->identifier);
	}else{
		return QString("espeak_VOICE{NULL}");
	}
}


static void listVoices(){
	auto voices=espeak_ListVoices(nullptr);
	int ct=0;
	if(nullptr!=voices){
		const espeak_VOICE *v;
		for(int ix=0; (v = voices[ix]) != nullptr; ix++){
			//qDebug()<<espeakVoiceToString(v);
			ct++;
		}
		qDebug()<<"GOT "<<ct<<" voices";
	}
	else{
		qWarning()<<"ERROR: Error fetching list of voices";
	}
}


template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::freeBuffer(Buffer<short> *buf)
{
	OC_METHODGATE();
	if(nullptr!=buf) {
		mBuffersInUse.removeAll(buf);
		mBuffersFree.append(buf);
	}
}


template <typename T, int OUTPUT_CHANNELS>
Buffer<short> *Voice<T, OUTPUT_CHANNELS>::getFreeBuffer(size_t numsamples)
{
	OC_METHODGATE();
	Buffer<short> *ret=nullptr;
	int i=0;
	for(auto buffer:mBuffersFree) {
		if(buffer->size == numsamples) {
			auto buffer = mBuffersFree.takeAt(i);
			if(nullptr != buffer) {
				ret = buffer;
				break;
			}
		}
		i++;
	}
	if(nullptr == ret) {
		ret = OC_NEW Buffer<short>(numsamples);
	}
	if(nullptr != ret) {
		mBuffersInUse.append(ret);
	}
	return ret;
}


template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::feed(short *wav, int numsamples)
{
	const size_t numsamples2{ (numsamples * af.sampleRate()) / (mHz == 0 ? 1 : mHz)};
	
	auto buffer=getFreeBuffer(numsamples2);
	if(nullptr!=buffer) {
		short * dp = buffer->buffer;
		if (nullptr != dp) {
			for (int i = 0; i < numsamples2; i++) {
				const int j = (i * numsamples) / numsamples2;
				dp[i] = wav[j];
			}
		}
	}
}



template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::deinit()
{
	OC_METHODGATE();
	if (isInitialized()) {
		espeak_Terminate();
	}
}


template <typename T, int OUTPUT_CHANNELS>
bool Voice<T, OUTPUT_CHANNELS>::speak(PortableID &id, QString text)
{
	OC_METHODGATE();
	if (isInitialized()) {
		qDebug().noquote().nospace() << "Speaking \"" << text << "\"\n";
		auto ret = espeak_Synth(text.toStdString().c_str(), static_cast<size_t>(text.size() + 1), 0, POS_CHARACTER, 0, espeakCHARS_AUTO, nullptr, static_cast<void *>(this));
		//espeak_Synchronize();
		qWarning()<<"speech ret was "<<QString::number(ret);
		return ret == EE_OK;
	}else{
		qWarning()<<"Calling speak while not initialized for:" << text;
	}
	return false;
}


template <typename T, int OUTPUT_CHANNELS>
bool Voice<T, OUTPUT_CHANNELS>::voiceIsAvailable()
{
	OC_FUNCTIONGATE();
	return true;
}


/*
double CreepyVoice::frandGauss()
{
	OC_METHODGATE();
	return rng->generateGauss();
}


double CreepyVoice::frandGaussAbs()
{
	OC_METHODGATE();
	return qAbs(rng->generateGauss());
}

*/

template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	OC_METHODGATE();
	af = f;
	espeak_VOICE idealVoice{
		"OctoMY-Voice" // a given name for this voice. UTF8 string.
		, "en-us" // list of pairs of (byte) priority + (string) language (and dialect qualifier)
		, nullptr // the filename for this voice within espeak-ng-data/voices
		, 1 // gender: 0=none 1=male, 2=female,
		, 42 // age: 0=not specified, or age in years
		, 0 // variant: only used when passed as a parameter to espeak_SetVoiceByProperties
		, 0 // for internal use
		, 0 // for internal use
		, nullptr // for internal use
	};
	
	const int rate=100; // static_cast<int>(frandGauss()*50.0+90)
	const int pitch=70; // static_cast<int>(frandGauss()*40.0+50)
	
	mHz = espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 10, nullptr, 0);
	if(EE_INTERNAL_ERROR == mHz) {
		qWarning()<<"ERROR: Could not initialize espeak";
		return;
	}
	qDebug()<<"Using voice "<<espeakVoiceToString(&idealVoice);
	listVoices();
	espeak_SetSynthCallback(Voice<T, OUTPUT_CHANNELS>::synthCallback);
	const espeak_ERROR err=espeak_SetVoiceByProperties(&idealVoice);
	if(EE_OK==err){
		/*
	 * cool
	espeak_SetParameter(espeakRATE, 100, 0);
	espeak_SetParameter(espeakPITCH, 20, 0);
	espeak_SetParameter(espeakVOLUME, 100, 0);
	espeak_SetParameter(espeakRANGE, 0, 0);
	*/
		espeak_SetParameter(espeakRATE, rate, 0);
		espeak_SetParameter(espeakPITCH, pitch, 0);
		espeak_SetParameter(espeakVOLUME, 150, 0);
		espeak_SetParameter(espeakRANGE, 0, 0);
		auto actualVoiceP=espeak_GetCurrentVoice();
		qDebug()<<"Set espeak voice by spec: "<<  espeakVoiceToString(&idealVoice)<<", actual="<<espeakVoiceToString(actualVoiceP);
	}
	else{
		qWarning()<<"ERROR: Could not set espeak voice by spec: "<<espeakErrorToString(err)<<" ("<<  espeakVoiceToString(&idealVoice) <<")";
	}
	mInited=true;
}



#else
///////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::feed(short *wav, int numsamples)
{
	Q_UNUSED(wav);
	Q_UNUSED(numsamples);
}




template <typename T, int OUTPUT_CHANNELS>
bool Voice<T, OUTPUT_CHANNELS>::speak(const PortableID &id, const QString &text) const
{
	OC_METHODGATE();
	if (isInitialized()) {
		qDebug().noquote().nospace() << "I am mute, not speaking \"" << text << "\"\n";
	}
}


template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::deinit()
{
	OC_METHODGATE();
	if (isInitialized()) {
	}
}


template <typename T, int OUTPUT_CHANNELS>
bool Voice<T, OUTPUT_CHANNELS>::voiceIsAvailable()
{
	OC_FUNCTIONGATE();
	return false;
}




template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	OC_METHODGATE();
	af = f;
}



template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::freeBuffer(Buffer<short> *buf)
{
	OC_METHODGATE();
	Q_UNUSED(buf);
}


#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////


template <typename T, int OUTPUT_CHANNELS>
T Voice<T, OUTPUT_CHANNELS>::generate()
{
	OC_METHODGATE();
	if(nullptr==cur) {
		if(mBuffersInUse.count()>0) {
			cur=mBuffersInUse.takeFirst();
			bac=0;
		}
	}
	T sam=0.0;
	if(nullptr!=cur) {
		sam=cur->buffer[bac];
		sam/=static_cast<T>(0xFFFF);
		bac++;
		if(bac>=cur->size) {
			freeBuffer(cur);
			cur=nullptr;
		}
		periodCounter=0;
	} else {
		//Leave 3 buffers worth of pause at the end to make sure the audio output system gets throug all the buffers before we shut it down.
		const size_t LIMIT = 3 * static_cast<size_t>(bufSize);
		if(periodCounter<LIMIT) {
			//qDebug()<<"Countind down " <<periodCounter;
			periodCounter++;
		}
		if(periodCounter==LIMIT) {
			periodCounter++;
			//emit periodComplete();
		}
		// Generate comfort noise
		// TODO: Make this actual pink noise instead of this sharp white noise
		sam=rng->generateReal1() * 0.02;
	}
	return sam;
}


template <typename T, int OUTPUT_CHANNELS>
void Voice<T, OUTPUT_CHANNELS>::generate(qint64 num, T *out)
{
	OC_METHODGATE();
	for(auto i=0; i<num; ++i){
		out[i]=generate();
	}
}




#endif // VOICE_HPP
