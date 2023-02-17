#ifndef WIDGETADI_H
#define WIDGETADI_H


#include <QWidget>
#include "qfi_ADI.h"
#include "LayoutSquare.h"


namespace Ui
{
class WidgetADI;
}


class WidgetADI : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString theme READ theme WRITE setTheme )

private:
	Ui::WidgetADI *m_ui;
	qfi_ADI       *m_adi;
	LayoutSquare  *m_layoutSq;

public:
	explicit WidgetADI( QWidget *parent = nullptr );
	~WidgetADI();

private:
	void setupUi();

public:
	void update();
	void setRoll( float roll );
	void setPitch( float pitch );
	void setTheme(QString theme);
	QString theme();
};


#endif
// WIDGETADI_H

