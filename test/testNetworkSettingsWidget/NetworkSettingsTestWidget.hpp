#ifndef NETWORKSETTINGSTESTWIDGET_HPP
#define NETWORKSETTINGSTESTWIDGET_HPP

#include <QWidget>

#include "basic/LocalAddressList.hpp"

namespace Ui
{
class NetworkSettingsTestWidget;
}

class NetworkSettingsTestWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::NetworkSettingsTestWidget *ui;
	LocalAddressList mAddresslist;

public:
	explicit NetworkSettingsTestWidget(QWidget *parent = 0);
	~NetworkSettingsTestWidget();

private slots:
	void on_pushButtonConfigure_clicked();
	void on_pushButtonToggleTimer_toggled(bool checked);

};

#endif // NETWORKSETTINGSTESTWIDGET_HPP
