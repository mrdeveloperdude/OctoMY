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

	bool verify(bool sendSignal=true);

	void updateTextView();

public slots:

	void onPortEditChanged();
	void onLocalAddressChanged(int index);

private slots:
	void on_pushButtonEdit_clicked();

	void on_pushButtonSave_clicked();

	signals:

	void validityChanged(bool);

};

#endif // NETWORKSETTINGSWIDGET_HPP
