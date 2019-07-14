#ifndef COMMSTESTER_HPP
#define COMMSTESTER_HPP

#include "Common_test.hpp"

#include "security/KeyStore.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include <QObject>
#include <QHostAddress>
#include <QSharedPointer>


class RNG;

class CommsTester:public QObject
{
	Q_OBJECT

public:
	QString mName;
	QHostAddress mMyAddress;
	quint16 mMyPort;

	quint16 mBasePort;
	quint16 mPortRange;
	QSharedPointer<CommsCarrierUDP> mCarrier;
	QSharedPointer<KeyStore> mKeyStore;
	QSharedPointer<AddressBook> mAssociates;
	QSharedPointer<CommsChannel> mCc;
	quint16 mTestCount;

	RNG *mRng;

public:
	explicit CommsTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, QSharedPointer<KeyStore> keyStore, QSharedPointer<AddressBook> peers, QObject *parent=nullptr);
	virtual ~CommsTester() {}

public:
	QString toString();

public slots:
	void startSendTest();
	void onError(QString);
	void onReadyRead();

signals:
	void finished();

};


#endif
// COMMSTESTER_HPP
