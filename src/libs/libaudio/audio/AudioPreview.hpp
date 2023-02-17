#ifndef AUDIOPROBEPREVIEW_HPP
#define AUDIOPROBEPREVIEW_HPP

#include <QElapsedTimer>
#include <QRect>

#include "AudioPreviewBase.hpp"
#include "audio/sinks/PitchDetector.hpp"

class QTimer;
class AudioPreviewSimple;

class QMediaRecorder;
class AudioPreview : public AudioPreviewBase
{
    Q_OBJECT
private:
    QSharedPointer<PitchDetector> mPitchDetector=nullptr;
    AudioPreviewSimple *mLevelWidget=nullptr;
    int mCurrentNote=0;
    bool mCurrentNotePresent=false;
    bool mDoDebug=false;

public:
    explicit AudioPreview(QWidget *parent = nullptr);
    ~AudioPreview() override;

private:
    void audioSetup(bool) override;
    void audioAvailable() override;

private:
    void drawWave(QPainter &painter, QRect rect);
    void drawNote(QPainter &painter, QRect rect);


private slots:
    void onPitchChanged(bool present, qreal note, qreal db);

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;


};

#endif // AUDIOPROBEPREVIEW_HPP
