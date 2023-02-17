#ifndef AUDIOGENERATOR_H
#define AUDIOGENERATOR_H

#include "random/RNG.hpp"
#include "SoftParam.hpp"


#include <QIODevice>
#include <QAudioFormat>


class AudioGenerator : public QIODevice
{
	Q_OBJECT
	
	Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
private:
	const bool mDoDebug=false;
	QAudioFormat mFormat;
	RNG *mRnd;
	SoftParam<float> mVolume;
	SoftParam<float> mPeriod;
	int mBufferSize=64; //16384/2;
	float mAngle=0;
	float (*mFun)(float) =&sinf;
	
public:
	AudioGenerator (float (*f)(float), QAudioFormat format, QObject* parent = nullptr);
	
public:
	void setPitch(float pitch);
	float pitch();
	
	void setVolume(float volume);
	float volume();
	
	inline float generate(){
		float fsam=mFun(mAngle)*mVolume.value();
		mAngle=fmod(mAngle+mPeriod.value(), M_2_PI);
		//consoleFloat(fsam);
		return fsam;
	}
	
	
public:
	bool isSequential() const override;
	qint64 readData(char *data, qint64 count) override;
	qint64 writeData(const char *data, qint64 count) override;
	qint64 bytesAvailable() const override;
	
signals:
	void volumeChanged(float volume);
	
};


#endif // AUDIOGENERATOR_H
