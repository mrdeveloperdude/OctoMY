#ifndef ADDRESSENTRY_HPP
#define ADDRESSENTRY_HPP

#include "comms/NetworkAddress.hpp"


#include <QSharedPointer>


struct AddressEntry {
public:
	NetworkAddress address;
	QString description;
	quint64 created;
	quint64 lastSuccess;
	quint64 lastError;
	quint64 numSuccessful;
	quint64 numErraneous;
public:

	explicit AddressEntry(NetworkAddress address, QString description="", quint64 created=0, quint64 lastSuccess=0, quint64 lastError=0, quint64 numSuccessful=0, quint64 numErraneous=0);
	explicit AddressEntry(QVariantMap map);
public:

	quint64 score(QHostAddress dgw = QHostAddress()) const;
	quint64 last() const;
	void tried(bool successful, quint64 now=0);

	QVariantMap toVariantMap() const;
	QString toString();

	bool operator==(const AddressEntry &o) const;
	bool operator!=(const AddressEntry &o) const;

};

const QDebug &operator<<(QDebug &d, AddressEntry &a);

#endif // ADDRESSENTRY_HPP
