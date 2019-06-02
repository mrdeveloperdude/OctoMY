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
	bool mLastPortOK;
	bool mLastAddressOK;
	bool mMuteSignals;

public:
	explicit NetworkSettingsWidget(QWidget *parent = nullptr);
	virtual ~NetworkSettingsWidget();

public:
	void configure(QSharedPointer<LocalAddressList> localAddresses);

private:
	bool isAddrOK(QHostAddress naddr);
	bool isPortOK(QHostAddress naddr, quint16 nport);
	bool verifyAndSet(bool sendSignal=true, bool doCorrection=true);

public:

	bool setHostAddress(QHostAddress naddr, quint16 nport, bool verify=true, bool sendSignal=true);
	bool setAddress(QHostAddress address, bool verify=true, bool sendSignal=true);
	bool setPort(quint16 port, bool verify=true, bool sendSignal=true);
	QHostAddress address() const;
	quint16 port() const;

	// ui slots
private slots:
	void on_pushButtonEdit_clicked();
	void on_pushButtonSave_clicked();
	void on_comboBoxLocalAddress_currentIndexChanged(int index);
	void on_lineEditLocalPort_textChanged(const QString &arg1);
	void on_lineEditLocalPort_editingFinished();

signals:
	void addressChanged(QHostAddress address, quint16 port, bool valid);

};

#endif
// NETWORKSETTINGSWIDGET_HPP
