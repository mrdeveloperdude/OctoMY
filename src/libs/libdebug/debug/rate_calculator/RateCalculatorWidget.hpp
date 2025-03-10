#ifndef RATECALCULATORWIDGET_H
#define RATECALCULATORWIDGET_H

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
	const RateCalculator *mRateCalculator{nullptr};

public:
	explicit RateCalculatorWidget(QWidget *parent = nullptr);
	~RateCalculatorWidget();

public:
	void configure(const RateCalculator *rc);

	void update();

};

#endif
// RATECALCULATORWIDGET_HPP
