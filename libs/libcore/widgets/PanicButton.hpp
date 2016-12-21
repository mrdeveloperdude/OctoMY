#ifndef PANICBUTTON_HPP
#define PANICBUTTON_HPP

#include <QWidget>

namespace Ui
{
class PanicButton;
}

class PanicButton : public QWidget
{
	Q_OBJECT

public:
	explicit PanicButton(QWidget *parent = 0);
	virtual ~PanicButton();

private:
	Ui::PanicButton *ui;

public:

	void setChecked(bool);

signals:
	void pressed();
	void released();
	void clicked(bool checked = false);
	void toggled(bool checked);

};

#endif // PANICBUTTON_HPP
