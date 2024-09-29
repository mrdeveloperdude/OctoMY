#ifndef PANICBUTTON_HPP
#define PANICBUTTON_HPP

#include <QWidget>
#include <QTimer>
#include <QAbstractButton>

namespace Ui
{
class PanicButton;
}

class PanicButton : public QAbstractButton
{
	Q_OBJECT
private:
	Ui::PanicButton *ui;
	QTimer mTimer;
	QPalette mPaletteNormal;
	QPalette mPaletteBlink;
	bool mFlipFlop;
public:
	explicit PanicButton(QWidget *parent = nullptr);
	virtual ~PanicButton();


public:

	void setPanic(bool);
	bool panic() const;


private slots:

	void onToggled(bool checked);
	void onTimeout();
/*
signals:
	void pressed();
	void released();
	void clicked(bool checked = false);
	void toggled(bool checked);
*/
protected:
	
	void paintEvent(QPaintEvent *) {};

};

#endif // PANICBUTTON_HPP
