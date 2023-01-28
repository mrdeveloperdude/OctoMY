#include "AudioPreviewBase.hpp"

#include "audio/utils/AudioUtils.hpp"
#include "audio/sources/Microphone.hpp"
#include "widgets/audio/AudioPreview.hpp"


#include <QCoreApplication>
#include <QDebug>
#include <QGradient>
#include <QIODevice>
#include <QKeyEvent>
#include <QPainter>
#include <QString>
#include <QTimer>
#include <QIcon>


AudioPreviewBase::AudioPreviewBase(QWidget *parent)
    : QWidget(parent)
{
    setWindowIcon(QIcon(":/data/vector/logo.svg"));
    setInput("");
    mCtor=false;
}

AudioPreviewBase::~AudioPreviewBase(){
}

void AudioPreviewBase::setInput(QString name){
    mName=name;
    QString windowTitle="Audio preview";
    if(!name.isEmpty())windowTitle+=QString(" for "+name);
    setWindowTitle(windowTitle);
    updateAudio();
}


void AudioPreviewBase::updateAudio(){
    if(!mMicrophone.isNull()){
        mMicrophone->deleteLater();
        mMicrophone=nullptr;
        mInitiated=false;
        if(!mCtor){
            audioSetup(false);
        }
    }
    if(!mName.trimmed().isEmpty() && isVisible()){
        mMicrophone=QSharedPointer<Microphone> (new Microphone);
        mMicrophone->configure(mName, baseAudioFormat);
        mFormat=mMicrophone->format();
        connect(mMicrophone.data(), &Microphone::samplesAvailable, this, &AudioPreviewBase::onSamplesAvailable, CONNECTION_TYPE);

        if(mFormat.isValid()){
            mInitiated=true;
            qDebug()<<"Using format:"<<mFormat;
            mSamples.reserve(width());
            if(!mCtor){
                audioSetup(true);
            }
        }
        else{
            qWarning()<<"Invalid format:"<<mFormat;
        }
    }
}


void AudioPreviewBase::onSamplesAvailable(QVector<float> samples)
{
    mSamples=samples;
    audioAvailable();
    update();
}

void AudioPreviewBase::keyPressEvent(QKeyEvent* event){
    if(nullptr!=this->parent()){
        return;
    }
    switch(event->key()){
        case(Qt::Key_Escape):this->deleteLater(); break;
        default:QWidget::keyPressEvent(event); break;
    };

}

void AudioPreviewBase::mouseReleaseEvent ( QMouseEvent *){
    if(nullptr!=this->parent()){
        if(!mName.trimmed().isEmpty()){
            auto ap=new AudioPreview(nullptr);
            ap->setInput(mName);
            int sz=320;
            ap->setMinimumSize(sz, (sz*9)/16);
            ap->show();
        }
    }
    else{
        this->deleteLater();
    }
}


void AudioPreviewBase::closeEvent(QCloseEvent *event){
   if (event->spontaneous()) {
       this->deleteLater();
   } else {
       event->ignore();
   }
}


void AudioPreviewBase::hideEvent(QHideEvent *)
{
    updateAudio();
}

void AudioPreviewBase::showEvent(QShowEvent *)
{
    updateAudio();
}
