#ifndef ARDUMYCONTROLLERWIDGET_HPP
#define ARDUMYCONTROLLERWIDGET_HPP

#include <QWidget>

class ArduMYController;


namespace Ui
{
class ArduMYControllerWidget;
}

class ArduMYControllerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ArduMYControllerWidget *ui;
	ArduMYController *mController;

public:
	explicit ArduMYControllerWidget(QWidget *parent = 0);
	~ArduMYControllerWidget();

public:
	void configure(ArduMYController *controller);

private slots:
	void on_pushButtonConfigureSerial_clicked();
	void on_pushButtonConfigureActuators_clicked();

	void on_spinBoxActuatorCount_valueChanged(int arg1);
};

#endif // ARDUMYCONTROLLERWIDGET_HPP
