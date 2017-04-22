#ifndef IAUDIOSOURCE_HPP
#define IAUDIOSOURCE_HPP



#include <QAudioFormat>

class IAudioProcessor{
	public:
		explicit IAudioProcessor(){}
		virtual ~IAudioProcessor(){}
		//IAudioProcessor interface
	public:
		virtual inline void init(QAudioFormat f)=0;
		virtual inline void process(qint64 num, double *in, double *out)=0;
};


class IAudioSource{
	public:
		explicit IAudioSource(){}
		virtual ~IAudioSource(){}
		//IAudioSource interface
	public:
		virtual inline void init(QAudioFormat f)=0;
		virtual inline void generate(qint64 num, double *out)=0;
};


#endif // IAUDIOSOURCE_HPP
