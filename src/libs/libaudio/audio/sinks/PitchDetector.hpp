#ifndef PITCHDETECTOR_HPP
#define PITCHDETECTOR_HPP

#include "audio/utils/MedianFilter.hpp"

#include <QObject>
#include <QSharedPointer>
#include <QAudioFormat>

#ifdef USE_PITCH_1
//#include "pitch/libpyincpp.h"
#endif
#ifdef USE_PITCH_2
//#include "pitch2/mfcc.h"
#endif
#ifdef USE_PITCH_3
#include "pitch3/analyzer.hpp"
#endif



class PitchDetector:public QObject{
    Q_OBJECT
private:

#ifdef USE_PITCH_1
    QSharedPointer<PyinCpp> mPyin=nullptr;
    Pitchlet mPitch;
#endif
#ifdef USE_PITCH_2
    QSharedPointer<MFCC<float> > mMFCC=nullptr;
#endif
#ifdef USE_PITCH_3
    QSharedPointer< Analyzer > mAnalyzer=nullptr;
#endif
    int  mNote=0;
    float mFreq=0;
    float mDb=0;
    bool mPitchPresent=false;
    MedianFilter<float, 7> mPitchMedianFilter;
    MedianFilter<int, 7> mNoteMedianFilter;
    MedianFilter<bool, 7> mPitchAvailableMedianFilter;
public:

    PitchDetector(QAudioFormat format, QObject *parent=nullptr);

public slots:

    void processBuffer1(QVector<float> &in);
    void processBuffer2(QVector<float> &in);
    void processBuffer3(QVector<float> &in);
    void processBuffer(QVector<float> &in);

signals:
    void pitchChanged(bool present, qreal note, qreal db);
};

#endif // PITCHDETECTOR_HPP
