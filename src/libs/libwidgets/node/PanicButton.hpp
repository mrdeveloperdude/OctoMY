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
	bool mPanic{false};
	bool mFlipFlop{false};

public:
	explicit PanicButton(QWidget *parent = nullptr);
	virtual ~PanicButton();

public:
	bool panic() const;

public slots:
	void setPanic(bool);

private slots:
	void timeout();

signals:
	void panicChanged(bool panic);
	
	// QAbstractButton interface
protected:
	void paintEvent(QPaintEvent *) override {};
};

#endif // PANICBUTTON_HPP
