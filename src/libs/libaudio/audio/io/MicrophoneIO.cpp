#include "MicrophoneIO.hpp"


#include "audio/utils/AudioUtils.hpp"

#include <QDebug>
#include <QAudioSource>
#include <QAudioInput>


MicrophoneIO::MicrophoneIO(QAudioFormat format)
    : mFormat(format)
{

}

qint64 MicrophoneIO::readData(char *data, qint64 maxlen){
    // Microphones are read only
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    return 0;
}

qint64 MicrophoneIO::writeData(const char *data, qint64 maxlen){
    auto ss=sampleSizeForFormat(mFormat);
    int num_samples = maxlen / (ss/8);
    mSamples.resize(num_samples);
    switch(ss){
        case(16):{
            auto ba=reinterpret_cast<const qint16 *>(data);
            for (int i = 0; i < num_samples; i ++) {
                mSamples[i]=ba[i]/32768.0f;
            }
        }break;
        case(8):{
            auto ba=reinterpret_cast<const qint8 *>(data);
            for (int i = 0; i < num_samples; i ++) {
                mSamples[i]=ba[i]/128.0f;
            }
        }break;
         default:{
            qWarning()<<"Microphone: Unsupported sample size (player audio):"<<ss<<"bits";
        }break;
    }
    emit samplesAvailable(mSamples);
    return maxlen;
}


