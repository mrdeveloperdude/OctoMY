#include "LevelDetector.hpp"

#include <QDebug>

#include <iostream>
#include <cmath>
#include <vector>


LevelDetector::LevelDetector(QAudioFormat, QObject *parent)
    : QObject(parent)
{
    emit levelChanged(0, 0, 0);
}


void LevelDetector::processBuffer(QVector<float> &in){
    float peakLevel = 0.0f;
    float sum = 0.0f;
    int ct=0;
    for( auto sample:in){
        peakLevel = std::fmax(peakLevel, sample);
        sum += sample * sample;
        ct++;
    }
    const auto rmsLevel = std::fmin(1.0f, std::fmax(0.0f, std::sqrt(sum / ct)));
    peakLevel=std::fmin(1.0f, std::fmax(0.0f,peakLevel));
    emit levelChanged(rmsLevel, peakLevel, ct);
}
