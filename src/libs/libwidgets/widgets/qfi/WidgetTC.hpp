#ifndef WIDGETTC_H
#define WIDGETTC_H


#include <QWidget>
#include "widgets/qfi/qfi_TC.h"
#include "LayoutSquare.h"


namespace Ui
{
class WidgetTC;
}


class WidgetTC : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString theme READ theme WRITE setTheme )

private:
	Ui::WidgetTC *m_ui;
	qfi_TC       *m_tc;
	LayoutSquare *m_layoutSq;

public:
	explicit WidgetTC( QWidget *parent = nullptr );
	~WidgetTC();

public:
	void update();
	void setTurnRate( float turnRate );
	void setSlipSkid( float slipSkid );
	void setTheme(QString theme);
	QString theme();

private:
	void setupUi();

};

#endif
// WIDGETTC_H
