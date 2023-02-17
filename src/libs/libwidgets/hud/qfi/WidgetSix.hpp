#ifndef WIDGETSIX_H
#define WIDGETSIX_H


#include <QWidget>

#include "WidgetADI.hpp"
#include "WidgetALT.hpp"
#include "WidgetASI.hpp"
#include "WidgetHSI.hpp"
#include "WidgetTC.hpp"
#include "WidgetVSI.hpp"


namespace Ui
{
class WidgetSix;
}

class WidgetSix : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString theme READ theme WRITE setTheme )

private:
	Ui::WidgetSix *m_ui;

	WidgetADI *m_widgetADI;
	WidgetALT *m_widgetALT;
	WidgetASI *m_widgetASI;
	WidgetHSI *m_widgetHSI;
	WidgetTC  *m_widgetTC;
	WidgetVSI *m_widgetVSI;

public:
	explicit WidgetSix( QWidget *parent = nullptr );
	~WidgetSix();

public:
	void update();
	void setRoll( float roll );
	void setPitch( float pitch );
	void setAltitude( float altitude );
	void setPressure( float pressure );
	void setAirspeed( float airspeed );
	void setHeading( float heading );
	void setTurnRate( float turnRate );
	void setSlipSkid( float slipSkid );
	void setClimbRate( float climbRate );
	void setTheme(QString theme);
	QString theme();

};

#endif
// WIDGETSIX_H
