#ifndef EYESWIDGET_HPP
#define EYESWIDGET_HPP

#include "expression/IrisRendrer.hpp"
#include "expression/EyeRendrer.hpp"

#include <QWidget>
#include <QTimer>
#include <QVector2D>
#include <QSharedPointer>
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
	qreal mBlink;
	qreal mCycle;
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

	bool mHideEyes;

public:
	explicit EyesWidget(QWidget *parent = nullptr);

public:
	void updateIris();
	void setPortableID(PortableID &pid);

protected:
	void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
	void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

public slots:
	void onUpdateTimer();
};

#endif // EYESWIDGET_HPP
