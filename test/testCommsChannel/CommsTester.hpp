#ifndef COMMSTESTER_HPP
#define COMMSTESTER_HPP

#include "security/KeyStore.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include <QObject>
#include <QHostAddress>
#include <QTest>

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
	CommsCarrierUDP mCarrier;
	KeyStore &mKeyStore;
	AddressBook &mAssociates;
	CommsChannel mCc;
	quint16 mTestCount;

	RNG *mRng;


public:
	explicit CommsTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, KeyStore &keyStore, AddressBook &peers, QObject *parent=nullptr);
	virtual ~CommsTester() {}
	QString toString();

public slots:
	void startSendTest();
	void onError(QString);
	void onReadyRead();
signals:
	void finished();

};


#endif // COMMSTESTER_HPP
