#ifndef LIGHTWIDGET_HPP
#define LIGHTWIDGET_HPP

#include <QWidget>
#include <QColor>
#include <QRadialGradient>

class LightWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool mOn READ isLightOn WRITE setLightOn)

private:
	QColor mLightColor;
	QColor mLightColorDarker;
	bool mOn;
	static const QColor sBgColor;
	static const QColor sHighlightColor;

	QRadialGradient mLightGradient;

public:
	LightWidget(QWidget *parent = nullptr,const QColor &mLightColorInner="#7cc043");

public:

	bool isLightOn() const;
	void setLightOn(bool mOn);
	void setLightColor(QColor c);
	void toggleLight();

public slots:
	void turnLightOff() ;
	void turnLightOn() ;

protected:
	virtual void paintEvent(QPaintEvent *);

};
#endif // LIGHTWIDGET_HPP
