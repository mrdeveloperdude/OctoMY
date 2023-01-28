#ifndef LOCALADDRESSLIST_HPP
#define LOCALADDRESSLIST_HPP


#include "uptime/ConfigureHelper.hpp"

#include <QList>
#include <QHostAddress>
#include <QTimer>
#include <QMap>
#include <QString>

/**
 * @brief The LocalAddressList class keeps track of the local addresses of a node.
 * The list contains many host addresses (IPs) but only one port, as the port
 * will not change in the same way as the hosts.
 *
 * + Hosts change by the whims of the network configuration on the underlying platform/OS
 * + Port changes when user selects a new port from the UI
 *
 */

class NetworkAddress;

class LocalAddressList: public QList<QHostAddress>
{
private:
	quint16 mPort;
	int mSelectedAddress;
	QObject *mObj;
	QTimer mTimer;
	ConfigureHelper mConfigureHelper;
	QMetaObject::Connection mTimeoutConnection;

public:
	explicit LocalAddressList();
	virtual ~LocalAddressList();

	// ConfigureHelper interface
public:
	void configure(quint16 port=0);
	void activate(const bool on);
	bool isActivated() const;

public:
	void setPort(quint16 port);
	quint16 port() const;

	void setCurrent(QHostAddress address, quint16 port=0);



	NetworkAddress currentNetworkAddress() const;
	QHostAddress currentAddress() const;

	bool isUpdateNeeded();
	bool updateIfNeeded(bool keepCurrent=false);
	void updateAddresses(bool keepCurrent=false);

	QString toString();
	QMap<QString, QString> toMap();
};

#endif
// LOCALADDRESSLIST_HPP
