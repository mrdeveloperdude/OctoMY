#ifndef ACTUATORCONTROLWIDGET_HPP
#define ACTUATORCONTROLWIDGET_HPP

#include <QWidget>



class Settings;

namespace Ui
{
class ActuatorControlWidget;
}

class ActuatorControlWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ActuatorControlWidget *ui;

	Settings *settings;

public:
	explicit ActuatorControlWidget(QWidget *parent = 0);
	~ActuatorControlWidget();

public:

	void configure(quint32 num);

public slots:

	void onServoMoved(quint32 id, qreal val);
	void onServoKilled(quint32 id);

};

#endif // ACTUATORCONTROLWIDGET_HPP
