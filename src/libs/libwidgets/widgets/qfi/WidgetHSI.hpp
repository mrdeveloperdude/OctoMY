#ifndef WIDGETHSI_H
#define WIDGETHSI_H


#include <QWidget>
#include "qfi_HSI.h"
#include "LayoutSquare.h"


namespace Ui
{
class WidgetHSI;
}


class WidgetHSI : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString theme READ theme WRITE setTheme )

private:
	Ui::WidgetHSI *m_ui;
	qfi_HSI       *m_hsi;
	LayoutSquare  *m_layoutSq;

public:
	explicit WidgetHSI( QWidget *parent = nullptr );
	~WidgetHSI();

public:
	void update();
	void setHeading( float heading );
	void setTheme(QString theme);
	QString theme();

private:
	void setupUi();
};


#endif
// WIDGETHSI_H
