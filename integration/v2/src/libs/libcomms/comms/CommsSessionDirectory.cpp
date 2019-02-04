#include "CommsSessionDirectory.hpp"

#include "uptime/MethodGate.hpp"

#include "CommsSession.hpp"
#include "security/KeyStore.hpp"
#include "security/Key.hpp"


CommsSessionDirectory::CommsSessionDirectory(KeyStore &keyStore)
	: QObject(nullptr)
	, mKeyStore(keyStore)
	, mRng(QSharedPointer<RNG>(RNG::sourceFactory("devu")))
	, mUnusedIndex(1)
{
	OC_METHODGATE();
}

CommsSessionDirectory::~CommsSessionDirectory()
{
	OC_METHODGATE();
}



void CommsSessionDirectory::insert(QSharedPointer<CommsSession> c)
{
	OC_METHODGATE();
	//CommsSignature &sig=c->signature();
	SESSION_ID_TYPE id=c->localSessionID();
	QString fullID=c->fullID();
	QString address=c->address().toString();
	if(nullptr==c) {
		qWarning()<<"ERROR: Trying to insert nullptr session";
	} else if(!c->address().isValid()) {
		qWarning()<<"ERROR: Trying to insert session with invalid address";
	} else if (mBySessionID.contains(id)) {
		qWarning()<<"ERROR: Trying to insert session with id "<<id<<" a second time";
	} else if(mByAddress.contains(address)) {
		qWarning()<<"ERROR: Trying to insert session with address "<<address<<" a second time";
	} else {
		mBySessionID.insert(id, c);
		mByFullID.insert(fullID, c);
		mByAddress.insert(address, c);
		mAll.insert(c);
		emit clientAdded(c);
	}
}

void CommsSessionDirectory::remove(QSharedPointer<CommsSession> c)
{
	OC_METHODGATE();
	if(nullptr!=c) {
		mByAddress.remove(c->address().toString());
		mByFullID.remove(c->fullID());
		mBySessionID.remove(c->localSessionID());
		mAll.remove(c);
	} else {
		qWarning()<<"ERROR: Trying to remove nullptr session";
	}
}


QSharedPointer<CommsSession> CommsSessionDirectory::bySessionID(const SESSION_ID_TYPE sessionID) const
{
	OC_METHODGATE();
	QHash<SESSION_ID_TYPE, QSharedPointer<CommsSession> >::const_iterator it=mBySessionID.find(sessionID);
	if(mBySessionID.end()==it) {
		return QSharedPointer<CommsSession>(nullptr);
	}
	return it.value();
}


QSharedPointer<CommsSession> CommsSessionDirectory::byFullID(const QString &id) const
{
	OC_METHODGATE();
	QHash<QString, QSharedPointer<CommsSession> >::const_iterator it=mByFullID.find(id);
	if(mByFullID.end()==it) {
		return QSharedPointer<CommsSession>(nullptr);
	}
	return it.value();
}

QSharedPointer<CommsSession> CommsSessionDirectory::byAddress(const NetworkAddress &address) const
{
	OC_METHODGATE();
	// Never accept bad address
	if(!address.isValid(false,false)) {
		return QSharedPointer<CommsSession>(nullptr);
	}
	QHash<QString, QSharedPointer<CommsSession> >::const_iterator it=mByAddress.find(address.toString());
	if(mByAddress.end()==it) {
		return QSharedPointer<CommsSession>(nullptr);
	}
	return it.value();
}



QSet<QSharedPointer<CommsSession> > CommsSessionDirectory::byActiveTime(quint64 lastActiveTime) const
{
	OC_METHODGATE();
	QSet<QSharedPointer<CommsSession> > ret;
	for(QSharedPointer<CommsSession> client:mAll) {
		if(nullptr!=client) {
			if(client->lastActiveTime()>=lastActiveTime) {
				ret << client;
			}
		}
	}
	return ret;
}


QSet<QSharedPointer<CommsSession> > CommsSessionDirectory::all() const
{
	OC_METHODGATE();
	return mAll;
}



bool CommsSessionDirectory::fullIDExists(const QString &id) const
{
	OC_METHODGATE();
	QHash<QString, QSharedPointer<CommsSession> >::const_iterator it=mByFullID.find(id);
	return (mByFullID.end()!=it);
}

QSet<QSharedPointer<CommsSession> > CommsSessionDirectory::byIdleTime(quint64 lastActiveTime) const
{
	OC_METHODGATE();
	QSet<QSharedPointer<CommsSession> > ret;
	for(QSharedPointer<CommsSession> client:mAll) {
		if(nullptr!=client) {
			if(client->lastActiveTime()<=lastActiveTime) {
				ret << client;
			}
		}
	}
	return ret;
}

int CommsSessionDirectory::count()
{
	OC_METHODGATE();
	return mAll.size();
}

int CommsSessionDirectory::countHandshakers()
{
	OC_METHODGATE();
	int ct=0;
	for(QSharedPointer<CommsSession> client:mAll) {
		if(nullptr!=client) {
			if((!client->established()) && (!client->expired())) {
				ct++;
			}
		}
	}
	return ct;
}


bool CommsSessionDirectory::hasHandshakers()
{
	OC_METHODGATE();
	for(QSharedPointer<CommsSession> client:mAll) {
		if(nullptr!=client) {
			if((!client->established()) && (!client->expired())) {
				return true;
			}
		}
	}
	return false;
}


quint32 CommsSessionDirectory::prune()
{
	OC_METHODGATE();
	quint32 ret=0;
	for (QSet<QSharedPointer<CommsSession> >::iterator it=mAll.begin(); it!=mAll.end(); ) {
		QSharedPointer<CommsSession> c=*it;
		if(nullptr!=c) {
			if(c->lastActiveTime() <= c->timeoutAccumulator()) {
				mByAddress.remove(c->address().toString());
				mByFullID.remove(c->fullID());
				mBySessionID.remove(c->localSessionID());
				it = mAll.erase(it);
				ret++;
			}
		} else {
			++it;
		}
	}
	return ret;
}

SESSION_ID_TYPE CommsSessionDirectory::generateUnusedSessionID()
{
	OC_METHODGATE();
	auto const m=1<<(sizeof(mUnusedIndex)*8-1);
	auto const m10=m/10;
	for(SESSION_ID_TYPE i=1; i<m; ++i) {
		// NOTE: Starting at 1 ensures that we never get INVALID_SESSION_ID by accident
		const SESSION_ID_TYPE  inc = 1 + (mRng->generateInt32() % m10);
		mUnusedIndex = (mUnusedIndex + inc)% m;
		if((0!= mUnusedIndex) && (mBySessionID.end() == mBySessionID.find(mUnusedIndex))) {
			// We have found an unused session
			return mUnusedIndex;
		}
	}
	// INVALID_SESSION_ID means "no unused sessions found"
	return INVALID_SESSION_ID;
}


QString CommsSessionDirectory::summary()
{
	OC_METHODGATE();
	QString out="CommsSessionDirectory with "+QString::number(mAll.count())+" sessions:\n";
	for(QSharedPointer<CommsSession> client:mAll) {
		if(nullptr!=client) {
			out += " + "+client->toString()+"\n";
		} else {
			out += " + null\n";
		}
	}
	return out;
}
