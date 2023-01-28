#include "Microphone.hpp"

#include "audio/utils/AudioUtils.hpp"

#include <QDebug>
#include <QAudioSource>
#include <QAudioInput>


Microphone::Microphone(QObject *parent)
    : QIODevice(parent)
{

}

void Microphone::setOutput(QIODevice *io){
    if(nullptr!=mIO){
        mIO->close();
    }
    if(io!=nullptr){
        mIO=io;
        if(!mAudioSource.isNull()){
            mAudioSource->stop();
            mAudioSource->reset();
            if(!mIO->open(QIODevice::ReadWrite)){
                qWarning()<<"Microphone: Could not open output:"<<mIO->errorString();
            }
            else{
                mAudioSource->start(mIO);
                if(mDoDebug){
                    qDebug()<<"Microphone: Started source";
                }
                //mIO = QSharedPointer<Microphone>(new MicrophoneIO(mFormat));
                //connect(mIO.data(), &Microphone::samplesAvailable, this, &Microphone::samplesAvailable, CONNECTION_TYPE);

            }
        }
    }
}

void Microphone::configure(QString name, QAudioFormat format){
    mName="";
    if(!mAudioSource.isNull()){
        mAudioSource=nullptr;
    }
    if(!mAudioInput.isNull()){
        mAudioInput=nullptr;
    }
    if(!name.trimmed().isEmpty()){
        auto device=audioDeviceForName(name, false);
        if(device.isNull()){
            qWarning()<<"Microphone: No device for name"<<name;
            qWarning()<<"Microphone: Available devices are:";
            auto devs=availableAudioDevices(format, false);
            for(auto &dev:devs){
                qWarning()<<" + "<<dev.description();
            }
            return;
        }
        mName=name;
        mFormat=format;
        if(mDoDebug)
            qDebug() << "Microphone: Using device" << device.description();
        if (!device.isFormatSupported(mFormat)) {
            qWarning() << "Microphone: Default format not supported, trying to use the nearest.";
            qWarning()<<"  Wanted:"<<mFormat;
            mFormat = device.preferredFormat();
            qWarning()<<"     Got:"<<mFormat;
        }
        auto as = new QAudioSource(device, mFormat);
        mAudioSource = QSharedPointer<QAudioSource>(as);
        connect(mAudioSource.data(), &QAudioSource::stateChanged, this, &Microphone::onStateChanged, CONNECTION_TYPE);
        auto ai=new QAudioInput();
        ai->setDevice(device);
        mAudioInput = QSharedPointer<QAudioInput>(ai);
        mAudioInput->setVolume(1.0);
        if(mDoDebug){
            qDebug()<<"Microphone: Prepared input";
        }
        setOutput(this);
    }
}

QAudioFormat Microphone::format(){
    return mFormat;
}


qint64 Microphone::readData(char *data, qint64 maxlen){
    qWarning()<<"Microphone: Error Read detected!"<<maxlen;
    // Microphones are read only
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    return 0;
}

qint64 Microphone::writeData(const char *data, qint64 maxlen){
    if(mDoDebug){
        qDebug()<<"Microphone: Write data"<<maxlen;
    }
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
            qWarning()<<"Microphone: Unsupported sample size (microphone):"<<ss<<"bits";
        }break;
    }
    emit samplesAvailable(mSamples);
    return maxlen;
}



void Microphone::onStateChanged(QAudio::State state){
    if(mDoDebug){
        qDebug()<<"Microphone: State changed to:"<<state;
    }
    switch (state) {
        case QAudio::IdleState:

            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (mAudioSource->error() != QAudio::NoError) {
                qWarning()<<"Microphone: Error:"<<mAudioSource->error();
            }
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}


qint64 Microphone::bytesAvailable() const {
    const qint64 bytes=64;
    if(mDoDebug){
        qDebug()<<"Microphone: bytesAvailable:"<<bytes;
    }
    return bytes;
}

qint64 Microphone::bytesToWrite() const {
    const qint64 bytes=64;
    if(mDoDebug){
        qDebug()<<"Microphone: bytesToWrite:"<<bytes;
    }
    return bytes;
}


bool Microphone::isSequential() const {
    const bool seq=true;
    if(mDoDebug){
        qDebug()<<"Microphone: isSequential:"<<seq;
    }
    return seq;
}
