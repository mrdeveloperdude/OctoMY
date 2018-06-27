#ifndef COLORSWATCHWIDGET_HPP
#define COLORSWATCHWIDGET_HPP

#include <QWidget>
#include <QTimer>

class ColorSwatchWidget : public QWidget
{
	Q_OBJECT
private:
	QColor mSwatchColor;

	bool mPressedInside;
public:
	explicit ColorSwatchWidget(QWidget *parent = nullptr);
	virtual ~ColorSwatchWidget();

	void setSwatchColor(QColor);
	QColor swatchColor();

	// Events inherited from QWidget
public:

	void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public:

	void paint(QPainter *painter, const QRect &rect, const QPalette &palette) const;


};

void paintColorSwatch(QColor c,QPainter *painter, const QRect &rect, const QPalette &) ;

Q_DECLARE_METATYPE(ColorSwatchWidget*)

#endif // COLORSWATCHWIDGET_HPP
