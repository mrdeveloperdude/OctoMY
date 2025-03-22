#ifndef MICROPHONEIO_HPP
#define MICROPHONEIO_HPP

#include <QString>
#include <QAudioFormat>
#include "uptime/SharedPointerWrapper.hpp"
#include <QAudioInput>
#include <QBuffer>


class QAudioSource;

class MicrophoneIO: public QIODevice{
    Q_OBJECT
private:
    QAudioFormat mFormat;
    QVector<float> mSamples;

public:
    MicrophoneIO(QAudioFormat format);

public:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 maxlen) override;

signals:
    // Notify that we have more samples on the read end
    void samplesAvailable(QVector<float> samples);

};


#endif // MICROPHONEIO_HPP
