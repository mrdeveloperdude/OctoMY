#ifndef NETWORKSETTINGSWIDGET_HPP
#define NETWORKSETTINGSWIDGET_HPP

#include <QWidget>

#include "../libcore/basic/NetworkAddress.hpp"

namespace Ui
{
class NetworkSettingsWidget;
}

class NetworkSettingsWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::NetworkSettingsWidget *ui;
public:
	explicit NetworkSettingsWidget(QWidget *parent = nullptr);
	virtual ~NetworkSettingsWidget();

public:

	NetworkAddress address();
	bool setPort(quint16);
	quint16 port();
	bool verify();

	void updateTextView();

public slots:

	void onPortEditChanged();
	void onLocalAddressChanged(int index);

	private slots:
	void on_pushButtonEditable_toggled(bool checked);
};

#endif // NETWORKSETTINGSWIDGET_HPP
