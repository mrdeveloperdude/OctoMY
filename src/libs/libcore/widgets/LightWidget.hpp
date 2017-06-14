#ifndef LIGHTWIDGET_HPP
#define LIGHTWIDGET_HPP

#include <QWidget>
#include <QColor>

class LightWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool on READ isLightOn WRITE setLightOn)

private:
	QColor color;
	bool on;
public:
	LightWidget(QWidget *parent = 0,const QColor &color="#7cc043");

public:

/*
	virtual int heightForWidth(int) const Q_DECL_OVERRIDE;
	virtual bool hasHeightForWidth() const Q_DECL_OVERRIDE;
*/

public:

	bool isLightOn() const;
	void setLightOn(bool on);
	void setLightColor(QColor c);
	void toggleLight();

public slots:
	void turnLightOff() ;
	void turnLightOn() ;

protected:
	virtual void paintEvent(QPaintEvent *);

};
#endif // LIGHTWIDGET_HPP
