#include "DecimalEntry.hpp"
#include "ui_DecimalEntry.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <cmath> // For std::round

DecimalEntry::DecimalEntry(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::DecimalEntry)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->doubleSpinBox->setFocusPolicy(Qt::StrongFocus);
	ui->horizontalSlider->setFocusPolicy(Qt::StrongFocus);
	
	if (!connect(ui->horizontalSlider, &QSlider::valueChanged, this, &DecimalEntry::sliderUpdate)) {
		qWarning() << "ERROR: Could not connect horizontalSlider signal.";
	}
	if (!connect(ui->doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &DecimalEntry::spinnerUpdate)) {
		qWarning() << "ERROR: Could not connect doubleSpinBox signal.";
	}
	// Set defaults
	configure(mMinValue, mMaxValue, (mMaxValue-mMinValue) * 0.5 + mMinValue);
}


DecimalEntry::~DecimalEntry()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
}


void DecimalEntry::configure(qreal min, qreal max, qreal val, QString suf, QString tip)
{
	OC_METHODGATE();
	mMinValue = qMin(min, max);
	mMaxValue = qMax(min, max);
	ui->doubleSpinBox->setRange(min, max);
	const auto dec = pow(10.0, ui->doubleSpinBox->decimals());
	ui->doubleSpinBox->setSingleStep(1.0 / dec);
	ui->doubleSpinBox->setSuffix(suf);
	ui->doubleSpinBox->setToolTip(tip);
	ui->horizontalSlider->setRange(static_cast<int>(mSliderResolution * min), static_cast<int>(mSliderResolution * max));
	ui->horizontalSlider->setPageStep(static_cast<int>(range()/dec));
	ui->horizontalSlider->setToolTip(tip);
	setValue(val);
	qDebug() << "DecimalEntry" << min<<max<<val << suf << tip<<mMinValue<<mMaxValue<<value()<<minimum()<<maximum();
}


void DecimalEntry::spinnerUpdate(const double val)
{
	OC_METHODGATE();
	if (val < mMinValue || val > mMaxValue) {
		qWarning() << "ERROR: Spinner value out of range.";
		return;
	}
	const auto intValue = static_cast<int>(std::round((val - mMinValue) / range() * ui->horizontalSlider->maximum()));
	QSignalBlocker sb(ui->horizontalSlider);
	ui->horizontalSlider->setValue(intValue);
}


void DecimalEntry::sliderUpdate(const int val)
{
	OC_METHODGATE();
	const auto decimalValue = mMinValue + (static_cast<qreal>(val) / ui->horizontalSlider->maximum()) * range();
	QSignalBlocker sb(ui->doubleSpinBox);
	ui->doubleSpinBox->setValue(decimalValue);
}


void DecimalEntry::setValue(const double val)
{
	OC_METHODGATE();
	const auto clean = qBound(mMinValue, val, mMaxValue);
	QSignalBlocker sb(ui->doubleSpinBox);
	ui->doubleSpinBox->setValue(clean);
	spinnerUpdate(clean);
}


double DecimalEntry::value()
{
	OC_METHODGATE();
	return ui->doubleSpinBox->value();
}


double DecimalEntry::minimum()
{
	OC_METHODGATE();
	return mMinValue;
}


double DecimalEntry::maximum()
{
	OC_METHODGATE();
	return mMaxValue;
}


double DecimalEntry::range()
{
	OC_METHODGATE();
	return mMaxValue - mMinValue;
}
