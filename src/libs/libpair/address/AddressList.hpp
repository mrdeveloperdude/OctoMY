#ifndef ADDRESSLIST_HPP
#define ADDRESSLIST_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QString>
#include <QHostAddress>

class NetworkAddress;
class AddressEntry;

/**
 * @brief The AddressList class is used to keep track of addresses that have been used by an associate.
 * It provides the means to iterate through the addresses based on a score calculated from
 * how many successfull vs. unsuccessfull connections have been made with this address and how long ago this occurred.
 *
 *
 * It works like this:
 * Everytime a new address is introduced, it is stored with the others with a blank statistic.
 * Whenever a connection is attemted, the failure/success is stored as a timestamp as well as counted into totals for successfull/unsuccessfull.
 *
 * statistics is persisted between runs.
 *
 * When the time comes to select which address to try, the list is copied into an imutable version that is iterated during the connection attempt, starting with the highest scoring entry and working down in loops.
 */
class AddressList: public QList<QSharedPointer<AddressEntry> >
{
public:
	explicit AddressList();
	explicit AddressList(QVariantList list);

public:
	void add(QSharedPointer<AddressEntry> address);
	void merge(NetworkAddress adr, QString description="", quint64 now=0);
	QSharedPointer<AddressEntry> highestScore(QHostAddress dgw=QHostAddress())  const;
	QMap<quint64, QSharedPointer<AddressEntry>> scoreMap(QHostAddress dgw=QHostAddress())  const;

	NetworkAddress bestAddress() const;

	QVariantList toVariantList() const;
	void  fromVariantList(QVariantList list);
	QString toString();

	bool isValid(bool allMustBeValid=false, bool allowLoopback=true, bool allowMulticast=false, bool allowIPv6=true);


public:
	bool operator== (const AddressList &b) const;
	bool operator!= (const AddressList &b) const;


};


const QDebug &operator<<(QDebug &d, AddressList &a);


#endif
// ADDRESSLIST_HPP
