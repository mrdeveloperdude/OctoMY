#include "Speaker.hpp"

#include "audio/utils/AudioUtils.hpp"

#include <QDebug>
#include <QAudioSink>
#include <QAudioOutput>
#include <cmath>


Speaker::Speaker(QObject *parent)
    : QIODevice(parent)
{

}

void Speaker::setInput(QIODevice *io){
    if(nullptr!=mIO){
        mIO->close();
    }
    if(io!=nullptr){
        mIO=io;
        if(!mAudioSink.isNull()){
            mAudioSink->stop();
            mAudioSink->reset();
            //connect(mIO.data(), &SpeakerIO::samplesNeeded, this, &Speaker::samplesNeeded, CONNECTION_TYPE);
            if(!mIO->open(QIODevice::ReadOnly)){
                qWarning()<<"Could not open input:"<<mIO->errorString();
            }
            else{
                mAudioSink->start(mIO);
                if(mDoDebug){
                    qDebug()<<"Speaker: Started sink";
                }
            }
        }
    }
    else if(mDoDebug){
        qDebug()<<"Speaker: No sink";
    }
}

void Speaker::configure(QString name, QAudioFormat format){
    mName="";
    /*
    if(!mAudioSink.isNull()){
        mAudioSink=nullptr;
    }
    if(!mAudioOutput.isNull()){
        mAudioOutput=nullptr;
    }
    */
    if(!name.trimmed().isEmpty()){
        auto device=audioDeviceForName(name, true);
        if(device.isNull()){
            qWarning()<<"Speaker: No device for name"<<name;
            qWarning()<<"Speaker: Available devices are:";
            auto devs=availableAudioDevices(format, true);
            for(auto &dev:devs){
                qWarning()<<" + "<<dev.description();
            }
            return;
        }
        mName=name;
        mFormat=format;
        if(mDoDebug){
            qDebug() << "Speaker: Using device" << device.description();
        }
        if (!device.isFormatSupported(mFormat)) {
            qWarning() << "Speaker: Default format not supported, trying to use the nearest.";
            qWarning()<<"  Wanted:"<<mFormat;
            mFormat = device.preferredFormat();
            qWarning()<<"     Got:"<<mFormat;
        }
        auto as = new QAudioSink(device, mFormat);
        mAudioSink = QSharedPointer<QAudioSink>(as);
        connect(mAudioSink.data(), &QAudioSink::stateChanged, this, &Speaker::onStateChanged, CONNECTION_TYPE);
        auto ao=new QAudioOutput();
        ao->setDevice(device);
        mAudioOutput = QSharedPointer<QAudioOutput>(ao);
        mAudioOutput->setVolume(1.0);
        if(mDoDebug){
            qDebug()<<"Speaker: Prepared output";
        }
        setInput(mIO);
    }
}

QAudioFormat Speaker::format(){
    return mFormat;

}

qint64 Speaker::readData(char *data, qint64 maxlen){
    if(mDoDebug){
        qDebug()<<"Speaker: Read data"<<maxlen;
    }
    qint64 len=0;
    if(false){
        auto s=reinterpret_cast<quint16 *>(data);
        //len= mBuffer.write(s, maxlen/2);
        len=maxlen;
    }
    else{
        len=maxlen;
    }
    if(len>0){
        emit samplesNeeded(len);
    }
    return len;
}


qint64 Speaker::writeData(const char *data, qint64 maxlen){
    qWarning()<<"Speaker: Error Write detected!"<<maxlen;
    // Speaker will never write data
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    return 0;
}


void Speaker::onStateChanged(QAudio::State state){
    if(mDoDebug){
        qDebug()<<"Speaker: State changed to:"<<state;
    }
    switch (state) {
        case QAudio::IdleState:

            break;

        case QAudio::StoppedState:
            if(nullptr!=mAudioSink){
                // Stopped for other reasons
                if (mAudioSink->error() != QAudio::NoError) {
                    qWarning()<<"Speaker: Error:"<<mAudioSink->error();
                }
            }
            else{
                qWarning()<<"Speaker: Error: no audiosink";
            }
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}


qint64 Speaker::bytesAvailable() const {
    const qint64 bytes=64;
    if(mDoDebug){
        qDebug()<<"Speaker: bytesAvailable:"<<bytes;
    }
    return bytes;
}

qint64 Speaker::bytesToWrite() const {
    const qint64 bytes=64;
    if(mDoDebug){
        qDebug()<<"Speaker: bytesToWrite:"<<bytes;
    }
    return bytes;
}

bool Speaker::isSequential() const {
    const bool seq=true;
    if(mDoDebug){
        qDebug()<<"Speaker: isSequential:"<<seq;
    }
    return seq;
}
