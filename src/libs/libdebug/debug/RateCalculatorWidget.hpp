#ifndef RATECALCULATORWIDGET_HPP
#define RATECALCULATORWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"
#include <QWidget>

class RateCalculator;
namespace Ui
{
class RateCalculatorWidget;
}

class RateCalculatorWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::RateCalculatorWidget *ui;
	ConfigureHelper mConfigureHelper;
	RateCalculator *mRateCalculator;

public:
	explicit RateCalculatorWidget(QWidget *parent = nullptr);
	~RateCalculatorWidget();

public:
	void configure(RateCalculator *rc);

	void update();

};

#endif
// RATECALCULATORWIDGET_HPP
