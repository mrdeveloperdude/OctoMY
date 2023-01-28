#include "SpeakerIO.hpp"


#include <QDebug>
#include <QAudioSink>
#include <QAudioOutput>
#include <cmath>


SpeakerIO::SpeakerIO(QAudioFormat format)
    : mFormat(format)
{

}

qint64 SpeakerIO::readData(char *data, qint64 maxlen){
    auto s=reinterpret_cast<quint16 *>(data);
    auto len= mBuffer.write(s, maxlen/2);
    if(len>0){
        emit samplesNeeded(len);
    }
    return len;
}


qint64 SpeakerIO::writeData(const char *data, qint64 len){
    // Speaker will never write data
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

