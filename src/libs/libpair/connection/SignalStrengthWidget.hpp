#ifndef SIGNALSTRENGTHWIDGET_HPP
#define SIGNALSTRENGTHWIDGET_HPP

#include <QWidget>

namespace Ui {
class SignalStrengthWidget;
}

class SignalStrengthWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SignalStrengthWidget(QWidget *parent = nullptr);
	~SignalStrengthWidget();

private:
	Ui::SignalStrengthWidget *ui;
};

#endif // SIGNALSTRENGTHWIDGET_HPP
