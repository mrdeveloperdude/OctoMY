#ifndef TRYSLIDER_HPP
#define TRYSLIDER_HPP

#include "TryValue.hpp"

#include <QWidget>


class TrySlider : public QWidget
{
	Q_OBJECT
private:
	TryValue<double> mValue;
public:
	explicit TrySlider(QWidget *parent = nullptr);

signals:
};

#endif // TRYSLIDER_HPP
