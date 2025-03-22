#ifndef AUDIOPREVIEWBASE_HPP
#define AUDIOPREVIEWBASE_HPP


#include "uptime/SharedPointerWrapper.hpp"

#include <QAudioFormat>
#include <QAudioInput>
#include <QBuffer>
#include <QTime>
#include <QWidget>


class QMouseEvent;
class Microphone;

class AudioPreviewBase : public QWidget
{
    Q_OBJECT
private:
    QSharedPointer<Microphone> mMicrophone=nullptr;
    bool mInitiated=false;
protected:
    QString mName;
    QAudioFormat mFormat;
    QVector<float> mSamples;
    bool mCtor=true;

public:
    explicit AudioPreviewBase(QWidget *parent = nullptr);
    virtual ~AudioPreviewBase();

public slots:
    void setInput(QString name);

private:
    void updateAudio();

private slots:

    void onSamplesAvailable(QVector<float> samples);
protected:

    virtual void audioSetup(bool) =0;
    virtual void audioAvailable() =0;

protected:
    void keyPressEvent(QKeyEvent *) override;
    void mouseReleaseEvent ( QMouseEvent * event ) override;
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *) override;
    void showEvent(QShowEvent *) override;

};


#endif // AUDIOPREVIEWBASE_HPP
