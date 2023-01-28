#ifndef LEVELDETECTOR_H
#define LEVELDETECTOR_H

#include <QObject>
#include <QSharedPointer>
#include <QAudioFormat>

class LevelDetector:public QObject{
    Q_OBJECT
private:

public:
    LevelDetector(QAudioFormat format, QObject *parent=nullptr);

public slots:
    void processBuffer(QVector<float> &in);

signals:
    void levelChanged(float  rmsLevel, float  peakLevel, int numSamples);
};




#endif // LEVELDETECTOR_H


