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

	void paintEvent(QPaintEvent *event) override;
	void hideEvent(QHideEvent *) override;
	void showEvent(QShowEvent *) override;
	void mousePressEvent(QMouseEvent *) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;
	void leaveEvent(QEvent *) override;
	void resizeEvent(QResizeEvent *event) override;

signals:

	void steeringChanged(qreal throttle, qreal steeringAngle);

};

#endif // CARSTEERINGWIDGET_HPP
