#ifndef AUDIOPREVIEWSIMPLE_H
#define AUDIOPREVIEWSIMPLE_H

#include "AudioPreviewBase.hpp"
#include "LevelDetector.hpp"


#include <QElapsedTimer>
#include "uptime/SharedPointerWrapper.hpp"
#include <QColor>


class QTimer;
class QMouseEvent;

struct SimpleLevelConfig{
    QColor background=QColor(0x222222);
    QColor rmsColor;
    QColor peakColor;
    QColor lowRmsColor1=QColor(0x51f700);
    QColor lowRmsColor2=lowRmsColor1;
    //QColor lowRmsColor2=QColor(0x2d8a00);
    QColor medRmsColor=QColor(0xfddc00);
    QColor highRmsColor=QColor(0xf85900);
    // Constants
    const int RedrawInterval = 1000/60; // ms
    const float PeakDecayRate = 0.0005f;
    const int PeakHoldLevelDuration = 500; // ms
    const float levelThreshold=-0.01f;
    const float decayInit=1.002;
    const float decayAcceleration=1.1;
};


class QMediaRecorder;
class AudioPreviewSimple : public AudioPreviewBase
{
    Q_OBJECT
public:
	enum DIRECTION{
		VERTICAL, HORIZONTAL, AUTO
	};
private:
	QSharedPointer<LevelDetector> mLevelDetector{nullptr};
    SimpleLevelConfig mLevelConfig;
	float mRmsLevel{0};
	float mPeakLevel{0};
	float mPeakHoldLevel{0};
	float mDecayedPeakLevel{0};
	float mAcceleration{2};
    QElapsedTimer mPeakLevelChanged;
    QElapsedTimer mPeakHoldLevelChanged;
	QTimer *mLevelRedrawTimer{nullptr};
    QBrush mHighBrush;
    QBrush mLowBrush;
	DIRECTION mDirection{AUTO};
	bool mHorizontal{false};
	bool mDoDrawDebug{false};
public:
    explicit AudioPreviewSimple(QWidget *parent = nullptr);
    ~AudioPreviewSimple() override;

public:
    void setDirection(DIRECTION=AUTO);

private:
	bool isHorizontal();
    void drawLevelVertical(QPainter &painter, QRect rect);
    void drawLevelHorizontal(QPainter &painter, QRect rect);

private:
    void audioSetup(bool) override;
    void audioAvailable() override;

private slots:
    void onLevelChanged(float  rmsLevel, float  peakLevel, int numSamples);
    void onLevelTimer();

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;


};


#endif // AUDIOPREVIEWSIMPLE_H
