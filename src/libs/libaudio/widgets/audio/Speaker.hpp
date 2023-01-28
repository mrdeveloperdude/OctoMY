#ifndef SPEAKER_HPP
#define SPEAKER_HPP


#include <QString>
#include <QAudioFormat>
#include <QSharedPointer>
#include <QBuffer>
#include <QAudio>

class QAudioSink;
class QAudioOutput;


class Speaker: public QIODevice
{
    Q_OBJECT
private:
    const bool mDoDebug=false;
    QAudioFormat mFormat;
    QString mName;

    QSharedPointer<QAudioSink> mAudioSink{nullptr};
    QSharedPointer<QAudioOutput> mAudioOutput{nullptr};
    QIODevice *mIO{nullptr};

public:
    Speaker(QObject *parent=nullptr);

public:
    void setInput(QIODevice *io);
    void configure(QString name, QAudioFormat format);
    QAudioFormat format();

private slots:
    void onStateChanged(QAudio::State state);

private:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    bool isSequential() const override;

signals:
    // Notify that we have a need for more samples
    void samplesNeeded(qint64 num);
};

#endif // SPEAKER_HPP
