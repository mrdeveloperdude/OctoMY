#ifndef SIGNALSTRENGTHWIDGET_HPP
#define SIGNALSTRENGTHWIDGET_HPP

#include <QWidget>

class SignalStrengthWidget : public QWidget
{
	Q_OBJECT
private:
	int mStrength{-1};
public:
	explicit SignalStrengthWidget(QWidget *parent = nullptr);
	~SignalStrengthWidget();
	
	QIcon icon();
	// QWidget interface:
protected:
	void paintEvent(QPaintEvent *e) override;
	
	
public slots:
	void signalStrengthChanged(int strength);

};

#endif // SIGNALSTRENGTHWIDGET_HPP
