#ifndef ACTUATORMANAGEMENTWIDGET_H
#define ACTUATORMANAGEMENTWIDGET_H

#include <QWidget>
#include <QList>

#include "uptime/SharedPointerWrapper.hpp"

class ControllerHandler;
class QSpacerItem;

namespace Ui
{
class ActuatorManagerWidget;
}

class ActuatorManagementWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ActuatorManagerWidget *ui;
	QList<QWidget *> mWidgets;
	QSharedPointer<ControllerHandler> mHandler;
	bool mDebug{false};

public:
	explicit ActuatorManagementWidget(QWidget *parent = nullptr);
	~ActuatorManagementWidget();

private:
	void updateWidgetCount(quint32 num);

public:
	void configure(QSharedPointer<ControllerHandler>);

private slots:
	void toggleLimpAll();
	void toggleEnableAll();

};

#endif
// ACTUATORMANAGERWIDGET_HPP

