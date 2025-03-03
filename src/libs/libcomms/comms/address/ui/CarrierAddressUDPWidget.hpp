#ifndef CARRIERADDRESSUDPWIDGET_HPP
#define CARRIERADDRESSUDPWIDGET_HPP

#include <QWidget>

namespace Ui {
class CarrierAddressUDPWidget;
}

class CarrierAddressUDPWidget : public QWidget
{
	Q_OBJECT

public:
	explicit CarrierAddressUDPWidget(QWidget *parent = nullptr);
	~CarrierAddressUDPWidget();

private:
	Ui::CarrierAddressUDPWidget *ui;
};

#endif // CARRIERADDRESSUDPWIDGET_HPP
