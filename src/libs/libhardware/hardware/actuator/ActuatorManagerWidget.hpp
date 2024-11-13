#ifndef ACTUATORMANAGERWIDGET_HPP
#define ACTUATORMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

#include "uptime/SharedPointerWrapper.hpp"

class ControllerHandler;
class QSpacerItem;

namespace Ui
{
class ActuatorManagerWidget;
}

class ActuatorManagerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ActuatorManagerWidget *ui;
	QList<QWidget *> mWidgets;
	QSharedPointer<ControllerHandler> mHandler;
	bool mDebug{true};

public:
	explicit ActuatorManagerWidget(QWidget *parent = nullptr);
	~ActuatorManagerWidget();

private:
	void updateWidgetCount(quint32 num);

public:
	void configure(QSharedPointer<ControllerHandler>);

public slots:
	void toggleLimpAll();
	void toggleEnableAll();

};

#endif
// ACTUATORMANAGERWIDGET_HPP

