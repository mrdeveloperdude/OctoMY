#ifndef PINGWIDGET_HPP
#define PINGWIDGET_HPP

#include <QWidget>

struct Ping{
	qint64 time;
	QColor color;
	bool leftSide;
	Ping(qint64 time, QRgb rgba, bool leftSide)
		: time(time)
		, color(QColor::fromRgba(rgba))
		, leftSide(leftSide)
	{
	}
	
	bool drawPing(QPainter &painter, const qint64 &now, const qreal &speed, const int &w, const int &h);
};

class PingWidget : public QWidget
{
	Q_OBJECT
private:
	qint64 mLastPing{-1};
	QColor mBackgroundColor{QColor::fromRgba(0x00000000)};
	qreal mSpeed{0.1};
	QList<Ping> mPings;

public:
	explicit PingWidget(QWidget *parent = nullptr);
	
	// QWidget interface
protected:
	void paintEvent(QPaintEvent *) override;
	
	
public slots:
	void ping(QRgb rgba=0xFF000000, bool leftSide=true);

};

#endif // PINGWIDGET_HPP
