#ifndef CARSTEERINGWIDGET_HPP
#define CARSTEERINGWIDGET_HPP

#include <QWidget>
#include <QTimer>
#include <QVector2D>

namespace Ui
{
class CarSteeringWidget;
}

class CarSteeringWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::CarSteeringWidget *ui;

	QTimer mUpdateTimer;
	QVector2D mLastMousePos;
	bool mLastMouseButton;
	QVector2D mLastMousePressPos;
	QVector2D mSz;
	qreal mWheelRadius;
	QVector2D mWheelCenter;
	qreal mLastAngle;
	qreal mLastThrottle;
	qreal mAngle;
	qreal mThrottle;


public:
	explicit CarSteeringWidget(QWidget *parent = nullptr);
	virtual ~CarSteeringWidget();

private:

	void updateState();
protected:

	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
	void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

signals:

	void steeringChanged(qreal throttle, qreal steeringAngle);

};

#endif // CARSTEERINGWIDGET_HPP
