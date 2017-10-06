#include "AddressList.hpp"

#include "AddressEntry.hpp"

#include "utility/Utility.hpp"

#include <QDateTime>

#include<QList>
#include <QMap>

//QSharedPointer<AddressEntry> >

AddressList::AddressList()
{

}


AddressList::AddressList(QVariantList list)
{
	for(QVariant var:list){
		QVariantMap map=var.toMap();
		mAll << QSharedPointer<AddressEntry>(new AddressEntry(map));
	}
}


void AddressList::add(QSharedPointer<AddressEntry> address)
{
	if(!address.isNull() && !address->address.isNull()) {
		mAll<<address;
	}
}

QSharedPointer<AddressEntry> AddressList::highestScore() const
{
	quint64 highestScore=0;
	QSharedPointer<AddressEntry> highestEntry;
	for(QSharedPointer<AddressEntry> entry:mAll) {
		if(!entry.isNull()) {
			const quint64 score = entry->score();
			if(score > highestScore) {
				highestScore = score;
				highestEntry = entry;
			}
		}
	}
	return highestEntry;
}

QMap<quint64, QSharedPointer<AddressEntry> > AddressList::scoreMap() const
{
	QMap<quint64, QSharedPointer<AddressEntry> > map;

	for(QSharedPointer<AddressEntry> entry:mAll) {
		if(!entry.isNull()) {
			const quint64 score = entry->score();
			map.insert(score, entry);
		}
	}
	return map;
}



quint64 AddressList::size() const
{
	return mAll.size();
}



QVariantList AddressList::toVariantList() const
{
	QVariantList list;
	for(QSharedPointer<AddressEntry> entry:mAll) {
		if(!entry.isNull()) {
			list<< entry->toVariantMap();
		}
	}
	return list;
}



QString AddressList::toString()
{
	OC_METHODGATE();
	QString out="";
	for(QSharedPointer<AddressEntry> entry:mAll) {
		if(entry.isNull()) {
			out+="null, ";
		} else {
			out+=entry->toString()+", ";
		}
	}
	return out;
}



const QDebug &operator<<(QDebug &d, AddressList &a)
{
	OC_FUNCTIONGATE();
	d.nospace() << "AddressList("<<a.toString()<<")";
	return d.maybeSpace();
}
