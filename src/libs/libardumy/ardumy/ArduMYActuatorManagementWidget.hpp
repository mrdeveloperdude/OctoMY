#ifndef ARDUMYACTUATORMANAGEMENTWIDGET_H
#define ARDUMYACTUATORMANAGEMENTWIDGET_H

#include <QWidget>
#include <QList>

#include "uptime/SharedPointerWrapper.hpp"

class IController;
class QSpacerItem;
class ArduMYController;
namespace Ui
{
class ArduMYActuatorManagementWidget;
}

class ArduMYActuatorManagementWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ArduMYActuatorManagementWidget *ui;
	QList<QWidget *> mWidgets;
	QSharedPointer<ArduMYController> mController;
	bool mDebug{true};

public:
	explicit ArduMYActuatorManagementWidget(QWidget *parent = nullptr);
	~ArduMYActuatorManagementWidget();

private:
	void updateWidgetCount(quint32 num);

public:
	void configure(QSharedPointer<IController>);

};

#endif
// ARDUMYACTUATORMANAGERWIDGET_HPP

