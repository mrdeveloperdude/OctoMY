#ifndef PANICBUTTON_HPP
#define PANICBUTTON_HPP

#include <QWidget>
#include <QTimer>

namespace Ui
{
class PanicButton;
}

class PanicButton : public QWidget
{
	Q_OBJECT
private:
	Ui::PanicButton *ui;
	QTimer mTimer;
	QPalette mPaletteNormal;
	QPalette mPaletteBlink;
	bool mFlipFlop;
public:
	explicit PanicButton(QWidget *parent = 0);
	virtual ~PanicButton();


public:

	void setPanic(bool);
	bool panic() const;


private slots:

	void onToggled(bool checked);
	void onTimeout();

signals:
	void pressed();
	void released();
	void clicked(bool checked = false);
	void toggled(bool checked);

};

#endif // PANICBUTTON_HPP
