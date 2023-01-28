#include "AudioPreviewSimple.hpp"

#include "audio/utils/AudioUtils.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QGradient>
#include <QIODevice>
#include <QKeyEvent>
#include <QPainter>
#include <QString>
#include <QTimer>

#include <cmath>


AudioPreviewSimple::AudioPreviewSimple(QWidget *parent)
    : AudioPreviewBase(parent)
    , mLevelRedrawTimer(new QTimer(this))
{
    mLevelRedrawTimer->setInterval(mLevelConfig.RedrawInterval);
    mLevelRedrawTimer->setTimerType(Qt::PreciseTimer);
    connect(mLevelRedrawTimer, &QTimer::timeout, this, &AudioPreviewSimple::onLevelTimer, CONNECTION_TYPE);
    mLevelRedrawTimer->start();
    mPeakLevelChanged.start();
    mPeakHoldLevelChanged.start();
    mAcceleration=mLevelConfig.decayInit;
}

AudioPreviewSimple::~AudioPreviewSimple(){
}


void AudioPreviewSimple::audioSetup(bool up){
    //qDebug()<<"UP? "<<up;
    if(up){
        mRmsLevel=0;
        mPeakLevel=0;
        mPeakHoldLevel=0;
        mDecayedPeakLevel=0;
        mLevelDetector=QSharedPointer<LevelDetector>(new LevelDetector(mFormat) );
        connect(mLevelDetector.data(), &LevelDetector::levelChanged, this, &AudioPreviewSimple::onLevelChanged, CONNECTION_TYPE);
    }
    else{
        if(!mLevelDetector.isNull()){
            mLevelDetector->deleteLater();
        }
        mLevelDetector=nullptr;
    }
}

void AudioPreviewSimple::setDirection(DIRECTION dir){
    mDirection=dir;
}

void AudioPreviewSimple::audioAvailable()
{
    if(!mLevelDetector.isNull()){
        mLevelDetector->processBuffer(mSamples);
    }
}


void AudioPreviewSimple::onLevelChanged(float  rmsLevel, float  peakLevel, int numSamples)
{
    //qDebug()<<"Level changed"<<rmsLevel<<peakLevel<<numSamples;
    // Smooth the RMS signal
    const float smooth = std::pow(0.9f, static_cast<float>(numSamples) / 128); // TODO: remove this magic number
    mRmsLevel = (mRmsLevel * smooth) + (rmsLevel * (1.0f - smooth));

    if (peakLevel > mDecayedPeakLevel) {
        mPeakLevel = peakLevel;
        mDecayedPeakLevel = peakLevel;
        mPeakLevelChanged.start();
    }

    if (peakLevel > mPeakHoldLevel) {
        mPeakHoldLevel = peakLevel;
        mPeakHoldLevelChanged.start();
    }
    update();
}


void AudioPreviewSimple::onLevelTimer()
{
    // Decay the peak signal
    const int elapsedMs = mPeakLevelChanged.elapsed();
    const float decayAmount = mLevelConfig.PeakDecayRate * elapsedMs;
    if (decayAmount < mPeakLevel){
        mDecayedPeakLevel = mPeakLevel - decayAmount;
        //qDebug()<<"LEVEL TIMER"<<mDecayedPeakLevel<<mPeakLevel << decayAmount<<elapsedMs<<mLevelConfig.PeakDecayRate;
    }
    else{
        mDecayedPeakLevel = 0.0f;
    }
    // Check whether to clear the peak hold level
    if (mPeakHoldLevelChanged.elapsed() > mLevelConfig.PeakHoldLevelDuration){
        if(mPeakHoldLevel>0.0f){
            mPeakHoldLevel -= mAcceleration-1.0;
            mAcceleration=std::pow(mAcceleration, mLevelConfig.decayAcceleration);
        }
    }
    else{
        mAcceleration=mLevelConfig.decayInit;
    }
    update();
}

bool AudioPreviewSimple::isHorizontal(){
	switch(mDirection){
	case(HORIZONTAL):return true;
	case(VERTICAL):return false;
	default:
		auto sz=size();
		return sz.width()>sz.height();
	}
}


void AudioPreviewSimple::drawLevelVertical(QPainter &painter, QRect rect){
    painter.fillRect(rect, mLevelConfig.background);
    QRect bar = rect;
    if(mDecayedPeakLevel > mLevelConfig.levelThreshold){
        // Peak Decay
        bar.setTop(rect.top() + floorint((1.0  - mDecayedPeakLevel) * rect.height()));
        painter.fillRect(bar, mLowBrush);
    }
    if(mRmsLevel>0.0f){
        // RMS
        bar.setTop(rect.top() + floorint((1.0  - mRmsLevel) * rect.height()));
        painter.fillRect(bar, mHighBrush);
    }
    if(mPeakHoldLevel>mDecayedPeakLevel && mPeakHoldLevel > mLevelConfig.levelThreshold){
        // Peak Hold (dot)
        bar.setTop(rect.top() + floorint((1.0 - mPeakHoldLevel) * rect.height()));
        bar.setBottom(bar.top() + 2);
        painter.fillRect(bar, mHighBrush);
    }
    if(mDoDrawDebug){
        painter.drawText(10,10,QString(" hold:%1").arg(mPeakHoldLevel,3));
        painter.drawText(10,25,QString("decay:%2").arg(mDecayedPeakLevel,3));
        painter.drawText(10,40,QString("  rms:%3").arg(mRmsLevel, 3));
    }
}


void AudioPreviewSimple::drawLevelHorizontal(QPainter &painter, QRect rect){
    painter.fillRect(rect, mLevelConfig.background);
    QRect bar = rect;
    if(mDecayedPeakLevel>mLevelConfig.levelThreshold){
        // Peak Decay
        bar.setRight(rect.right() - floorint((1.0  - mDecayedPeakLevel) * rect.width()));
        painter.fillRect(bar, mLowBrush);
    }

    if(mRmsLevel>0.0f){
        // RMS
        bar.setRight(rect.right() - floorint((1.0  - mRmsLevel) * rect.width()));
        painter.fillRect(bar, mHighBrush);
    }
    if(mPeakHoldLevel>mDecayedPeakLevel && mPeakHoldLevel > mLevelConfig.levelThreshold){
        // Peak Hold (dot)
        bar.setRight(rect.right() - floorint((1.0 - mPeakHoldLevel) * rect.width()));
        bar.setLeft(bar.right() + 2);
        painter.fillRect(bar, mHighBrush);
    }
    if(mDoDrawDebug){
        painter.drawText(10,10,QString(" hold:%1").arg(mPeakHoldLevel,3));
        painter.drawText(10,25,QString("decay:%2").arg(mDecayedPeakLevel,3));
        painter.drawText(10,40,QString("  rms:%3").arg(mRmsLevel, 3));
    }
}



void AudioPreviewSimple::resizeEvent(QResizeEvent *) {
    auto const r=rect();
    const qreal highPos=0.20;
    const qreal medPos= 0.40;
    const qreal edge=   0.001;
	mHorizontal=isHorizontal();
	
    auto highGradient=mHorizontal?QLinearGradient(r.width(), 0,0, 0):QLinearGradient(0, 0, 0, r.height());
    highGradient.setColorAt(0.0, mLevelConfig.highRmsColor);
    highGradient.setColorAt(highPos, mLevelConfig.highRmsColor);
    highGradient.setColorAt(highPos+edge, mLevelConfig.medRmsColor);
    highGradient.setColorAt(medPos, mLevelConfig.medRmsColor);
    highGradient.setColorAt(medPos+edge, mLevelConfig.lowRmsColor1);
    highGradient.setColorAt(1.0, mLevelConfig.lowRmsColor2);
    mHighBrush=QBrush(highGradient);
    auto lowGradient=mHorizontal?QLinearGradient(r.width(), 0,0, 0):QLinearGradient(0, 0, 0, r.height());
    const int dm=150;
    lowGradient.setColorAt(0.0, mLevelConfig.highRmsColor.darker(dm));
    lowGradient.setColorAt(highPos, mLevelConfig.highRmsColor.darker(dm));
    lowGradient.setColorAt(highPos+edge, mLevelConfig.medRmsColor.darker(dm));
    lowGradient.setColorAt(medPos, mLevelConfig.medRmsColor.darker(dm));
    lowGradient.setColorAt(medPos+edge, mLevelConfig.lowRmsColor1.darker(dm));
    lowGradient.setColorAt(1.0, mLevelConfig.lowRmsColor2.darker(dm));
    mLowBrush=QBrush(lowGradient);
}


void AudioPreviewSimple::paintEvent(QPaintEvent *){
    QPainter painter(this);
    antialiasHints(painter);
    //Level
    QRect levelRect=rect();
    if(mHorizontal){
        drawLevelHorizontal(painter, levelRect);
    }
    else{
        drawLevelVertical(painter, levelRect);
    }
}

