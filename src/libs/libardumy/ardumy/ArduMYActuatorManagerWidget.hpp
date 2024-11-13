#ifndef ARDUMYACTUATORMANAGERWIDGET_HPP
#define ARDUMYACTUATORMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

#include "uptime/SharedPointerWrapper.hpp"

class IController;
class QSpacerItem;
namespace Ui
{
class ArduMYActuatorManagerWidget;
}

class ArduMYActuatorManagerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ArduMYActuatorManagerWidget *ui;
	QList<QWidget *> mWidgets;
	QSharedPointer<IController> mController;
	bool mDebug{true};

public:
	explicit ArduMYActuatorManagerWidget(QWidget *parent = nullptr);
	~ArduMYActuatorManagerWidget();

private:
	void updateWidgetCount(quint32 num);

public:
	void configure(QSharedPointer<IController>);

};

#endif
// ARDUMYACTUATORMANAGERWIDGET_HPP

