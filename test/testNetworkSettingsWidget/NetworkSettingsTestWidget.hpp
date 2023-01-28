#ifndef NETWORKSETTINGSTESTWIDGET_HPP
#define NETWORKSETTINGSTESTWIDGET_HPP

#include <QWidget>
#include <QSharedPointer>

#include "address/LocalAddressList.hpp"

namespace Ui
{
class NetworkSettingsTestWidget;
}


class NetworkSettingsTestWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::NetworkSettingsTestWidget *ui;
	QSharedPointer<LocalAddressList> mAddresslist;

public:
	explicit NetworkSettingsTestWidget(QWidget *parent = nullptr);
	~NetworkSettingsTestWidget();

private slots:
	void on_pushButtonConfigure_clicked();
	void on_pushButtonToggleTimer_toggled(bool checked);

};

#endif
// NETWORKSETTINGSTESTWIDGET_HPP
