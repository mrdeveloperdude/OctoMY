#ifndef DECIMALENTRY_HPP
#define DECIMALENTRY_HPP

#include <QWidget>

namespace Ui {
class DecimalEntry;
}

class DecimalEntry : public QWidget
{
	Q_OBJECT
	
private:
	Ui::DecimalEntry *ui;
	qreal mMinValue{0.0};
	qreal mMaxValue{1.0};
	static const int mSliderResolution{1000};

public:
	explicit DecimalEntry(QWidget *parent = nullptr);
	~DecimalEntry();

public:
	void configure(qreal min, qreal max, qreal val, QString suf = "", QString tip = "");

	double value();
	void setValue(double val);

	double minimum();
	double maximum();
	double range();

private slots:
	void spinnerUpdate(double val);
	void sliderUpdate(int val);
};

#endif // DECIMALENTRY_HPP
