#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <QString>
#include <QAudioFormat>
#include "uptime/SharedPointerWrapper.hpp"
#include <QAudioInput>
#include <QBuffer>


class QAudioSource;

class Microphone: public QIODevice
{
    Q_OBJECT
private:
    const bool mDoDebug=false;
    QAudioFormat mFormat;
    QVector<float> mSamples;
    QString mName;

    QSharedPointer<QAudioSource> mAudioSource = nullptr;
    QSharedPointer<QAudioInput> mAudioInput = nullptr;
    QIODevice *mIO = nullptr;

public:
    Microphone(QObject *parent=nullptr);

public:
    void setOutput(QIODevice *io);
    void configure(QString name, QAudioFormat format);
    QAudioFormat format();

public:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 maxlen) override;

    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    bool isSequential() const override;

private slots:
        void onStateChanged(QAudio::State state);

signals:
    // Notify that we have more samples
    void samplesAvailable(QVector<float> samples);

};

#endif // MICROPHONE_H
