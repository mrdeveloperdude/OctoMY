#include "AudioPreview.hpp"

#include "widgets/audio/AudioPreviewSimple.hpp"

#include "audio/utils/AudioUtils.hpp"
#include "audio/utils/NoteUtils.hpp"

#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QIODevice>
#include <QString>
#include <QGradient>


static const QPen colors[]{
     QPen(Qt::green)
   , QPen(Qt::red)
   , QPen(Qt::blue)
   , QPen(Qt::yellow)
   , QPen(Qt::cyan)
   , QPen(Qt::magenta)
};



AudioPreview::AudioPreview(QWidget *parent)
    : AudioPreviewBase(parent)
{
    mLevelWidget=new AudioPreviewSimple(this);
}

AudioPreview::~AudioPreview(){
    if(nullptr!=mLevelWidget){
        mLevelWidget->setParent(nullptr);
        mLevelWidget->deleteLater();
        mLevelWidget=nullptr;
    }
}

void AudioPreview::audioSetup(bool up){
    if(up){
        mPitchDetector=QSharedPointer<PitchDetector>(new PitchDetector(mFormat) );
        connect(mPitchDetector.data(), &PitchDetector::pitchChanged, this, &AudioPreview::onPitchChanged, CONNECTION_TYPE);
        if(nullptr!=mLevelWidget){
            mLevelWidget->setInput(mName);
        }
    }
    else{
        if(!mPitchDetector.isNull()){
            mPitchDetector->deleteLater();
        }
        mPitchDetector=nullptr;
    }
}


void AudioPreview::audioAvailable()
{
    if(!mPitchDetector.isNull()){
        mPitchDetector->processBuffer(mSamples);
    }
}


void AudioPreview::drawWave(QPainter &painter, QRect rect){
    auto w=rect.width();
    auto h=rect.height();
    //auto w2=w/2;
    auto h2=h/2;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // Background
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,w,h);
    // Waveform
    //painter.setCompositionMode(QPainter::CompositionMode_Plus);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::green);
    auto frameCount=mSamples.size();
    float lastSample=0.0;
    float step=w/static_cast<float>(frameCount);
    //qDebug()<<"step:"<<step<<"count:"<<count<<"w:"<<w;
    for(int frame=0;frame<frameCount;++frame){
        auto x=frame*step;
        auto currentSample=mSamples[frame];
        painter.drawLine(floorint(x-1),floorint(h2+lastSample*h2), floorint(x), floorint(h2+currentSample*h2));
        lastSample=currentSample;
    }
}


void AudioPreview::drawNote(QPainter &painter, QRect){
    //Pitch
    if(mCurrentNotePresent){
        QFont font("sans");
        int base=10;
        font.setPixelSize(base);
        QFontMetrics fm(font);
        auto str=midiNoteToString(mCurrentNote);
        QRect bb(0,0,fm.horizontalAdvance(str), fm.height());
        QRect clip(2, 2, width() - 4, height() - 4);
        if(aspect(bb)>aspect(clip)){
            base*=clip.width();
            base/=bb.width();
        }else{
            base*=clip.height();
            base/=bb.height();
        }
        painter.setPen(Qt::white);
        font.setPixelSize(base);
        painter.setFont(font);
        painter.drawText(clip, Qt::AlignCenter | Qt::TextSingleLine, str);
    }
}

void AudioPreview::resizeEvent(QResizeEvent *) {
    if(nullptr!=mLevelWidget){
        auto r=rect();
        const int part=8;
        const int inset=5;
        QRect levelRect((r.width()*(part-1))/part, 0,r.width()/part, r.height());
        levelRect.adjust(inset, inset, -inset, -inset);
        mLevelWidget->setGeometry(levelRect);
    }
}

void AudioPreview::paintEvent(QPaintEvent *){
    QPainter painter(this);
    drawWave(painter, rect());
}


void AudioPreview::onPitchChanged(bool present, qreal note, qreal){
    mCurrentNotePresent=present;
    mCurrentNote=note;
}


