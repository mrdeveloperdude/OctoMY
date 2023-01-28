#ifndef SPEAKERIO_HPP
#define SPEAKERIO_HPP

#include "audio/utils/Inchworm.hpp"

#include <QString>
#include <QAudioFormat>
#include <QSharedPointer>
#include <QBuffer>
#include <QAudio>

class QAudioSink;
class QAudioOutput;



class SpeakerIO: public QIODevice{
    Q_OBJECT
private:
    QAudioFormat mFormat;
    Inchworm<64, quint16> mBuffer;

public:
    SpeakerIO(QAudioFormat format);

private:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

signals:
    // Notify that we have a need for more samples
    void samplesNeeded(qint64 num);
};




#endif // SPEAKERIO_HPP
