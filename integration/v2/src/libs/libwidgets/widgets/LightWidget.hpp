#ifndef LIGHTWIDGET_HPP
#define LIGHTWIDGET_HPP

#include <QWidget>
#include <QColor>
#include <QRadialGradient>
#include <QTimer>

class LightWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool mOn READ isLightOn WRITE setLightOn)

private:
	static const QColor sBgColor;
	static const QColor sHighlightColor;

public:
	static const QColor sDefaultOKColor;
	static const QColor sDefaultErrorColor;

private:
	QColor mLightColor;
	QColor mLightColorDarker;
	bool mOn;
	QRadialGradient mLightGradient;
	QTimer mBlinkTimer;

public:
	LightWidget(QWidget *parent = nullptr,const QColor &mLightColorInner=sDefaultOKColor);

public:
	bool isLightOn() const;
	void setLightOn(bool mOn);
	void setLightColor(QColor c);
	void toggleLight();

public slots:
	void turnLightOff();
	void turnLightOn();
	void blink(int ms);

protected:
	virtual void paintEvent(QPaintEvent *);

};

#endif
// LIGHTWIDGET_HPP
