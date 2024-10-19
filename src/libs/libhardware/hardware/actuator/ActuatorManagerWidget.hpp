#ifndef ACTUATORMANAGERWIDGET_HPP
#define ACTUATORMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

class IController;
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
	IController *mController;

public:
	explicit ActuatorManagerWidget(QWidget *parent = nullptr);
	~ActuatorManagerWidget();

private:
	void updateWidgetCount(quint32 num);

public:
	void configure(IController *);

public slots:
	void controllerSettingsChanged();

};

#endif
// ACTUATORMANAGERWIDGET_HPP

