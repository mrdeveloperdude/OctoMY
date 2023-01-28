#include "AudioGenerator.hpp"

#include "audio/utils/AudioUtils.hpp"

#include <QDebug>



AudioGenerator::AudioGenerator (float (*f)(float), QAudioFormat format, QObject* parent)
    : QIODevice(parent)
    , mFormat(format)
	, mRnd(RNG::sourceFactory("mt"))
{
    if(mDoDebug){
        qDebug()<<"AG CTOR";
    }
    mFun=f;
    // Start with A4
    setPitch(440.0);
    setVolume(1.0);
}


bool AudioGenerator::isSequential() const
{
    if(mDoDebug){
        qDebug()<<"AG SEQUENTIAL";
    }
    return true;
}

qint64 AudioGenerator::bytesAvailable() const {
    //qDebug()<<"AG"<<"BYTES AVAILABLE"<<count;
    return mBufferSize;
}


qint64 AudioGenerator::readData(char *data, qint64 maxlen)
{
    auto ss=sampleSizeForFormat(mFormat);
    int num_samples = (ss>=0)?(maxlen / (ss/8)):0;
    if(mDoDebug){
        qDebug()<<"AG READ DATA"<<maxlen<<"("<<num_samples<<" samples)";
    }
    switch(ss){
        case(16):{
            auto s=reinterpret_cast<qint16 *>(data);
            for (int i = 0; i < num_samples; i ++) {
                auto fsam=generate();
                s[i]=floorint(fsam*(0x7fff-1));
            }
        }break;
        case(8):{
            auto s=reinterpret_cast<qint8 *>(data);
            for (int i = 0; i < num_samples; i ++) {
                auto fsam=generate();
                s[i]=floorint(fsam*(0x7f-1));
            }
        }break;
         default:{
            qWarning()<<"Unsupported sample size (AudioGenerator):"<<ss<<"bits";
        }break;
    }
    return maxlen;
}

qint64 AudioGenerator::writeData(const char *data, qint64 count){
    Q_UNUSED(data);
    qWarning()<<"AG ERROR: WRITE DATA"<<count;
    return count;
}


void AudioGenerator::setPitch(float pitch){
    if(mDoDebug){
        qDebug()<<"AG"<<"PITCH SET"<<pitch;
    }
    float period=(pitch*TAU)/mFormat.sampleRate();
    mPeriod.setTarget(period);
}

float AudioGenerator::pitch(){
    return (mPeriod.target*mFormat.sampleRate())/TAU;
}


void AudioGenerator::setVolume(float volume){
    if(mDoDebug){
        qDebug()<<"AG"<<"VOLUME SET"<<volume;
    }
    mVolume.setTarget(volume);
    emit volumeChanged(volume);
}

float AudioGenerator::volume(){
    return mVolume.target;
}
