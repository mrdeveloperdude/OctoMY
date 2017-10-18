#ifndef NETWORKSETTINGSWIDGET_HPP
#define NETWORKSETTINGSWIDGET_HPP

#include <QWidget>

#include "comms/NetworkAddress.hpp"


class LocalAddressList;
class QHostAddress;

namespace Ui
{
class NetworkSettingsWidget;
}

class NetworkSettingsWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::NetworkSettingsWidget *ui;
	QHostAddress mLastAddress;
	quint16 mLastPort;
	bool mLastValidity;
public:
	explicit NetworkSettingsWidget(QWidget *parent = nullptr);
	virtual ~NetworkSettingsWidget();

private:
	bool set(bool sendSignal=true);

public:
	void configure(LocalAddressList &localAddresses);
	bool set(QHostAddress naddr, quint16 nport, bool sendSignal=true);
	bool setAddress(QHostAddress address);
	bool setPort(quint16 port);

	QHostAddress address() const;
	quint16 port(bool *ok=nullptr) const;

private slots:

	// ui slots
	void on_pushButtonEdit_clicked();
	void on_pushButtonSave_clicked();

	void on_comboBoxLocalAddress_currentIndexChanged(int index);

	void on_lineEditLocalPort_textChanged(const QString &arg1);

	signals:

	void addressChanged(QHostAddress address, quint16 port, bool valid);

};

#endif // NETWORKSETTINGSWIDGET_HPP
