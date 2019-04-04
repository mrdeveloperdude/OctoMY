#ifndef LOCALADDRESSLIST_HPP
#define LOCALADDRESSLIST_HPP

#include <QList>
#include <QHostAddress>
#include <QTimer>

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
public:
	explicit LocalAddressList();
	virtual ~LocalAddressList();

public:
	void configure(quint16 port=0, bool startTimer=false);


public:
	void setPort(quint16 port);
	quint16 port() const;

	void setCurrent(QHostAddress address, quint16 port=0);

	void setTimerEnabled(bool enabled);
	bool timerEnabled() const;

	NetworkAddress currentNetworkAddress() const;
	QHostAddress currentAddress() const;

	bool isUpdateNeeded();
	bool updateIfNeeded(bool keepCurrent=false);
	void updateAddresses(bool keepCurrent=false);
};

#endif
// LOCALADDRESSLIST_HPP
