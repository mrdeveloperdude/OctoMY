#include "PitchDetector.hpp"

#include "audio/utils/NoteUtils.hpp"

#include <QDebug>
#include <QAudioFormat>

#include <iostream>
#include <cmath>
#include <vector>


// MFCC Config
#define numCepstra 12
#define numFilters 40
#define winLength 25
#define frameShift  10
#define lowFreq 50


PitchDetector::PitchDetector(QAudioFormat format, QObject *parent)
    : QObject(parent)
#ifdef USE_PITCH_1
    //, mPyin(new PyinCpp(format.sampleRate(), 2048, 512))
    , mPyin(nullptr)
#endif
#ifdef USE_PITCH_2
    // Initialise MFCC class instance
    //, mMFCC(new MFCC<float> (format.sampleRate(), numCepstra, winLength, frameShift, numFilters, lowFreq, format.sampleRate()/2))
    , mMFCC(nullptr)
#endif
#ifdef USE_PITCH_3
    // Initialize pitch 3
    , mAnalyzer(new Analyzer(format.sampleRate(), "1"))
#endif
{
	
	Q_UNUSED(format);
#ifdef USE_PITCH_1
    //qDebug()<<"Pitch detector CTOR";
    if(!mPyin.isNull()){
        qDebug()<<"PITCH-1 enabled";
        mPyin->setCutOff(2);
    }
#endif
#ifdef USE_PITCH_2
    if(!mMFCC.isNull()){
        qDebug()<<"PITCH-2 enabled";
    }
#endif
#ifdef USE_PITCH_3
    if(!mAnalyzer.isNull()){
        qDebug()<<"PITCH-3 enabled";
    }
#endif
}

void PitchDetector::processBuffer1(QVector<float> &in){
    Q_UNUSED(in);
    #ifdef USE_PITCH_1
    if(!mPyin.isNull()){
        auto instd=in.toStdVector();
        //auto lastPitch=mPitch;
        //mPitch = mPyin->feed(instd);
        mPitch = mPyin->feed(in);
        auto lastNote=mNote;
        auto lastPitchPresent=mPitchPresent;

        mPitchPresent=!mPitchAvailableMedianFilter.Insert(!(mPitch.max_prob<3));

        if(mPitchPresent){
            mNote=freqToNote(mPitch.max_pitch);
            mNote=mPitchMedianFilter.Insert(mNote);
            //auto freq=noteToFreq(mNote);
            //auto nowPitch=400-freq;
        }
        auto change=((mNote !=lastNote) || (mPitchPresent != lastPitchPresent) );
        if(change){
            //qDebug()<<"PITCH SIGNAL: "<<mPitchPresent << mNote;
            emit pitchChanged(mPitchPresent, mNote,0);
        }
    }
#endif
}

void PitchDetector::processBuffer2(QVector<float> &in){
    Q_UNUSED(in);
    #ifdef USE_PITCH_2
    if(!mMFCC.isNull()){
        auto lol=mMFCC->processFrameFloat(in);
        //qDebug().noquote().nospace()<<QString::fromStdString(lol);
        /*
        auto change=((mNote !=lastNote) || (mPitchPresent != lastPitchPresent) );
        if(change){
            //qDebug()<<"PITCH SIGNAL: "<<mPitchPresent << mNote;
            emit noteChanged(mPitchPresent, mNote);
        }
        */
    }
    #endif
}

void PitchDetector::processBuffer3(QVector<float> &in){
    Q_UNUSED(in);
    #ifdef USE_PITCH_3
    if(!mAnalyzer.isNull()){
        mAnalyzer->input(in.begin(), in.end());
        mAnalyzer->process();
        //auto tones = mAnalyzer->getTones();
        auto tone = mAnalyzer->findTone();
//        auto lastFreq=mFreq;
//        auto lastPitchPresent=mPitchPresent;
        mPitchPresent=false;
        float thresholdDb=-30;
        mDb=thresholdDb-1;
        if(nullptr!=tone){
            if(tone->db > thresholdDb){
                mPitchPresent=true;
            }
            mFreq=tone->freq;
            mDb=tone->db;
            mFreq=mPitchMedianFilter.Insert(mFreq);
        }
        //auto change=((mFreq !=lastFreq) || (mPitchPresent != lastPitchPresent) );
        //if(change){
        if(mPitchPresent){
            //auto f=noteToFreq(62);            auto b=freqToNote(f);
            //qDebug()<<"PITCH SIGNAL: "<<mPitchPresent << mFreq<<mDb;
            emit pitchChanged(mPitchPresent, mFreq, mDb);
        }
        /*
        for(auto tone: tones){
            qDebug()<<"TONE:"<<tone.tostr();
        }
        */
    }
#endif
}

void PitchDetector::processBuffer(QVector<float> &in){
    //processBuffer1(in);
    //processBuffer2(in);
    processBuffer3(in);
}
