#ifndef COMMSSESSIONDIRECTORY_HPP
#define COMMSSESSIONDIRECTORY_HPP

#include "NetworkAddress.hpp"
#include "CommsSessionIDType.hpp"
#include "rng/RNG.hpp"

#include <QObject>
#include <QHash>
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
		// TODO: Maybe this dependency on keystore could be dropped?
	KeyStore &mKeyStore;
	QHash<SESSION_ID_TYPE, QSharedPointer<CommsSession> > mBySessionID;
	QHash<QString, QSharedPointer<CommsSession> > mByFullID;
	QHash<QString, QSharedPointer<CommsSession> > mByAddress;
	QSet<QSharedPointer<CommsSession> > mAll;
	QSharedPointer<RNG> mRng;
	SESSION_ID_TYPE mUnusedIndex;

public:

	explicit CommsSessionDirectory(KeyStore &keyStore);
	virtual ~CommsSessionDirectory();
public:

	void insert(QSharedPointer<CommsSession> c);
	void remove(QSharedPointer<CommsSession> c);
	QSharedPointer<CommsSession> bySessionID(const SESSION_ID_TYPE id) const;
	QSharedPointer<CommsSession> byFullID(const QString &id) const;
	QSharedPointer<CommsSession> byAddress(const NetworkAddress &address) const;
	QSet<QSharedPointer<CommsSession> > byActiveTime(quint64 lastActiveTime) const;
	QSet<QSharedPointer<CommsSession> > byIdleTime(quint64 lastActiveTime) const;
	QSet<QSharedPointer<CommsSession> > all() const;


	bool fullIDExists(const QString &id) const;

	int count();
	bool hasHandshakers();
	int countHandshakers();

	// Remove expired sessions to conserve resources.
	// May be called from a timer or on event
	quint32 prune();



public:
	SESSION_ID_TYPE generateUnusedSessionID(); // Return a random session ID that is unused in this directory

public:

	QString summary();

signals:
	void clientAdded(QSharedPointer<CommsSession> );


};


#endif // COMMSSESSIONDIRECTORY_HPP
