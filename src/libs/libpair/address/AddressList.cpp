#include "AddressList.hpp"

#include "AddressEntry.hpp"
#include "comms/address/CarrierAddress.hpp"
#include "comms/address/CarrierAddressUDP.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QDateTime>
#include <QList>
#include <QMap>


AddressList::AddressList()
{
	OC_METHODGATE();
}


AddressList::AddressList(QVariantList list)
{
	OC_METHODGATE();
	fromVariantList(list);
}


void AddressList::add(QSharedPointer<AddressEntry> address)
{
	OC_METHODGATE();
	if(!address.isNull() && !address->address.isNull() ) {
		if(!address->address->isValid()) {
			qWarning()<<"WARNING: Not adding invalid address: "<<address->address;
		} else {
			*this<<address;
		}
	}
}

void AddressList::merge(QSharedPointer<CarrierAddress> adr, QString description, quint64 now)
{
	OC_METHODGATE();
	//Never put bad addresses into the list
	if(adr.isNull()) {
		qWarning()<<"WARNING: Skipping null address while merging";
		return;
	}
	if(!adr->isValid()) {
		qWarning()<<"WARNING: Skipping bad address "<<adr->toString()<<" while merging";
		return;
	}
	if(0 == now) {
		now = utility::time::currentMsecsSinceEpoch<quint64>();
	}
	for(auto &entry:*this) {
		if(!entry.isNull()) {
			// Already inn, no need to continue
			if(entry->address == adr) {
				return;
			}
		}
	}
	*this<<QSharedPointer<AddressEntry>(new AddressEntry(adr, description, now));
}

QSharedPointer<AddressEntry> AddressList::highestScore(QHostAddress dgw) const
{
	OC_METHODGATE();
	quint64 highestScore=0;
	QSharedPointer<AddressEntry> highestEntry;
	for(auto &entry:*this) {
		if(!entry.isNull()) {
			const quint64 score = entry->score(dgw);
			if(score > highestScore) {
				highestScore = score;
				highestEntry = entry;
			}
		}
	}
	return highestEntry;
}

QMap<quint64, QSharedPointer<AddressEntry> > AddressList::scoreMap(QHostAddress dgw) const
{
	OC_METHODGATE();
	QMap<quint64, QSharedPointer<AddressEntry> > map;

	for(auto &entry:*this) {
		if(!entry.isNull()) {
			const quint64 score = entry->score(dgw);
			map.insert(score, entry);
		}
	}
	return map;
}


QSharedPointer<CarrierAddress> AddressList::bestAddress() const
{
	OC_METHODGATE();
	QSharedPointer<AddressEntry>  hs = highestScore();
	if(!hs.isNull()) {
		return hs->address;
	}
	return nullptr;

}



QVariantList AddressList::toVariantList() const
{
	OC_METHODGATE();
	QVariantList list;
	for(auto &entry:*this) {
		if(!entry.isNull()) {
			list<< entry->toVariantMap();
		}
	}
	return list;
}

void AddressList::fromVariantList(QVariantList list)
{
	OC_METHODGATE();
	clear();
	for(auto &var:list) {
		auto map{var.toMap()};
		add(QSharedPointer<AddressEntry>(new AddressEntry(map)));
	}
}


QString AddressList::toString()
{
	OC_METHODGATE();
	QString out="";
	for(auto &entry:*this) {
		if(entry.isNull()) {
			out+="null, ";
		} else {
			out+=entry->toString()+", ";
		}
	}
	return out;
}


bool AddressList::isValid(bool allMustBeValid, bool allowLoopback, bool allowMulticast, bool allowIPv6)
{
	OC_METHODGATE();
	const int sz=size();
	if(0==sz) {
		qWarning()<<"0 == list.sz";
		return false;
	}
	int ct=0;
	for(auto entry:*this) {
		if(entry.isNull()) {
			continue;
		} else {
			auto address = entry->address;
			if(address.isNull()){
				continue;
			}
			switch(address->type()){
				case UDP:{
					auto udp_address = qSharedPointerCast<CarrierAddressUDP>(address);
					if(!udp_address->isValidIP(allowLoopback, allowMulticast, allowIPv6)){
						continue;
					}
				}break;
				default:
				case LOCAL:
				case INVALID_CARRIER_ADDRESS:{
					if (!entry->address->isValid()) {
						continue;
					}
				}break;
			}
			ct++;
		}
	}
	if(allMustBeValid) {
		if(ct != sz) {
			qWarning() << "ct(" << ct << ") != list.sz(" << sz << ")";
			return false;
		}
	} else {
		if(ct<1) {
			qWarning() << "ct(" << ct << ") < 1";
			return false;
		}
	}
	return true;
}



bool AddressList::operator== (const AddressList &b) const
{
	for(QSharedPointer<AddressEntry> ae:b) {
		//TODO: Implement this if it is ever needed
	}
	return false;
}


bool AddressList::operator!= (const AddressList &b) const
{
	Q_UNUSED(b);
	//TODO: Implement this if it is ever needed
	return false;
}



const QDebug &operator<<(QDebug &d, AddressList &a)
{
	OC_FUNCTIONGATE();
	d.nospace() << "AddressList("<<a.toString()<<")";
	return d.maybeSpace();
}
