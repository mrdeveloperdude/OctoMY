#ifndef ACTUATORMANAGERWIDGET_HPP
#define ACTUATORMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

class ArduMYActuatorSet;
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
	ArduMYActuatorSet *mActuators;

public:
	explicit ActuatorManagerWidget(QWidget *parent = 0);
	~ActuatorManagerWidget();


private:

	void updateWidgetCount(quint32 num);
public:

	void configure(ArduMYActuatorSet *);

public slots:

	void onServoMoved(quint32 id, qreal val);
	void onServoLimped(quint32 id);

};

#endif // ACTUATORMANAGERWIDGET_HPP
