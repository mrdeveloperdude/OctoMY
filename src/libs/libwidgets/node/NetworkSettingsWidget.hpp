#ifndef NETWORKSETTINGSWIDGET_HPP
#define NETWORKSETTINGSWIDGET_HPP

#include <QWidget>
#include <QHostAddress>


class LocalAddressList;

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
	bool mDebug{false};

public:
	explicit NetworkSettingsWidget(QWidget *parent = nullptr);
	virtual ~NetworkSettingsWidget();

public:
	void configure(QSharedPointer<LocalAddressList> localAddresses);

private:
    bool isAddrToBeAdded(QHostAddress naddr);
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
	void edit();
	void save();
	void localPortTextChanged(const QString &string);
	void localPortEditingFinished();
	void localAddressIndexChanged(int index);
	
signals:
	void addressChanged(QHostAddress address, quint16 port, bool valid);

};

#endif
// NETWORKSETTINGSWIDGET_HPP
