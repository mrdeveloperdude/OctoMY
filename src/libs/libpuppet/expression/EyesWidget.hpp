#ifndef EYESWIDGET_HPP
#define EYESWIDGET_HPP

#include "expression/IrisRendrer.hpp"
#include "expression/EyeRendrer.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>
#include <QTimer>
#include <QVector2D>
#include <QImage>


class Personality;
class PortableID;
class Identicon;

class EyesWidget : public QWidget
{
	Q_OBJECT
private:
	QTimer mTimer;
	quint64 mStartTime;
	quint64 mLastTime;
	float mBlink;
	float mCycle;
	EyeRendrer mLeftEye;
	EyeRendrer mRightEye;
	QBrush mBgBrush;

	QVector2D upperLidSteer;
	QVector2D lowerLidSteer;
	QVector2D mEyeSteer;
	QVector2D mEyeSteerSmooth;
	QVector2D squintSteer;

	QVector2D lastPress;

	IrisRendrer mIrisRendrer;
	
	bool mEyesVisible{false};
	bool mDebug{false};

public:
	explicit EyesWidget(QWidget *parent = nullptr);

public:
	void updateIris();
	void setPortableID(PortableID &pid);

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
