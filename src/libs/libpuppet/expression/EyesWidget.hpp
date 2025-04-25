#ifndef EYESWIDGET_HPP
#define EYESWIDGET_HPP

#include "expression/IrisRendrer.hpp"
#include "expression/EyeRendrer.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>
#include <QTimer>
#include <QVector2D>
#include <QImage>


/*
Inspiration for next version:

marble https://www.shadertoy.com/view/Mlj3zW
cartoon eyes https://www.shadertoy.com/view/4tlXWX
realistic blitz games https://www.shadertoy.com/view/XsfGWj
petrification https://www.shadertoy.com/view/MsKBDG
alien cute https://www.shadertoy.com/view/Wss3Df

*/


class Personality;
class PortableID;
class Identicon;


class EyesWidget : public QWidget
{
	Q_OBJECT
private:
	QTimer mTimer;
	quint64 mStartTime{0};
	quint64 mLastTime{0};
	qreal mBlink{0.0};
	qreal mCycle{0.0};
	EyeRendrer mLeftEye;
	EyeRendrer mRightEye;
	QBrush mBgBrush;
	qreal mAttention{0.0};
	
	QVector2D mUpperLidSteer;
	QVector2D mLowerLidSteer;
	QVector2D mAttentionEyeSteer;
	QVector2D mEyeSteer;
	QVector2D mEyeSteerSmooth;
	QVector2D mSquintSteer;

	QVector2D lastPress;

	IrisRendrer mIrisRendrer;
	
	bool mEyesVisible{false};
	bool mDebug{false};

public:
	explicit EyesWidget(QWidget *parent = nullptr);

public:
	void setPortableID(PortableID &pid);
	void updateIrisImage();
	void updateSteerFromGlobal();
	void updateAttention();
	void updateEyeShape();

protected:
	void paintEvent(QPaintEvent *) override;
	void hideEvent(QHideEvent *) override;
	void showEvent(QShowEvent *) override;
	void mousePressEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;
	void leaveEvent(QEvent *) override;
	void resizeEvent(QResizeEvent *) override;

public slots:
	void onUpdateTimer();
};

#endif
// EYESWIDGET_HPP
