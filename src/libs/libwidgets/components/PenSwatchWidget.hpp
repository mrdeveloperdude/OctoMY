#ifndef PENSWATCHWIDGET_HPP
#define PENSWATCHWIDGET_HPP

#include <QWidget>
#include <QTimer>
#include <QPen>

class PenSwatchWidget : public QWidget
{
	Q_OBJECT
private:
	QPen mSwatchPen;

	bool mPressedInside;
public:
	explicit PenSwatchWidget(QWidget *parent = nullptr);
	virtual ~PenSwatchWidget();

	void setSwatchPen(QPen);
	QPen swatchPen();

	// Events inherited from QWidget
public:

	void paintEvent(QPaintEvent *) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

public:

	void paint(QPainter *painter, const QRect &rect, const QPalette &palette) const;


};

void paintPenSwatch(QPen pen, QPainter *painter, const QRect &rect, const QPalette &) ;

Q_DECLARE_METATYPE(PenSwatchWidget*)

#endif // PENSWATCHWIDGET_HPP
