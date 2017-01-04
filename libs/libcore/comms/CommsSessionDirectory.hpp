#ifndef COMMSSESSIONDIRECTORY_HPP
#define COMMSSESSIONDIRECTORY_HPP

#include "basic/NetworkAddress.hpp"
#include "comms/CommsSignature.hpp"

#include <QObject>
#include <QMap>
#include <QSet>
#include <QSharedPointer>


class CommsSession;
class KeyStore;

/**
  * \brief CommsSessionDirectory holds all session beknownst to CommChannel, both
  * un-initialized, active and terminated.
  */

class CommsSessionDirectory: public QObject
{
	Q_OBJECT
private:
	KeyStore &mKeyStore;
	QMap<quint64, QSharedPointer<CommsSession> > mByShortID;
	QMap<QString, QSharedPointer<CommsSession> > mByFullID;
	QMap<QString, QSharedPointer<CommsSession> > mByHost;
	QSet<QSharedPointer<CommsSession> > mAll;
public:

	explicit CommsSessionDirectory(KeyStore &keyStore);
	virtual ~CommsSessionDirectory();
public:

	void insert(QSharedPointer<CommsSession> c);
	QSharedPointer<CommsSession> getByShortID(const quint64 id);
	QSharedPointer<CommsSession> getByFullID(const QString &id, const bool addIfMissing=false);
	//QSharedPointer<CommsSession> getByAddress(const NetworkAddress &address, const bool addIfMissing=false);
	QSharedPointer<CommsSession> getBySignature(const CommsSignature &sig, const bool addIfMissing=false);

	QSet<QSharedPointer<CommsSession> > getByActiveTime(quint64 lastActiveTime);

	int count();

	// Remove expired sessions to conserve resources.
	// May be called from a timer or on event
	quint32 prune();

signals:
	void clientAdded(QSharedPointer<CommsSession> );


};


#endif // COMMSSESSIONDIRECTORY_HPP
