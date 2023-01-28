#ifndef IAUDIO_H
#define IAUDIO_H


#include <QAudioFormat>

template <typename T, int OUTPUT_CHANNELS = 2>
class IAudioProcessor
{
public:
	explicit IAudioProcessor() {}
	virtual ~IAudioProcessor() {}
	//IAudioProcessor interface
public:
	virtual void init(QAudioFormat f)=0;
	virtual void process(qint64 num, T *in, T *out)=0;
};


template <typename T, int OUTPUT_CHANNELS = 2>
class IAudioSource
{
public:
	explicit IAudioSource() {}
	virtual ~IAudioSource() {}
	//IAudioSource interface
public:
	virtual void init(QAudioFormat f)=0;
	virtual void generate(qint64 num, T *out)=0;
};


#endif
// IAUDIOSOURCE_HPP
 
