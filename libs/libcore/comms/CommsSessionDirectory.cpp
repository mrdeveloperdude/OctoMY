#include "CommsSessionDirectory.hpp"

#include "CommsSession.hpp"
#include "security/KeyStore.hpp"
#include "security/Key.hpp"



CommsSessionDirectory::CommsSessionDirectory(KeyStore &keyStore)
	: QObject(nullptr)
	, mKeyStore(keyStore)
{

}

CommsSessionDirectory::~CommsSessionDirectory()
{

}



void CommsSessionDirectory::insert(QSharedPointer<CommsSession> c)
{
	CommsSignature &sig=c->signature();
	quint64 id=sig.shortHandID();
	QString fullID=sig.fullID();
	QString address=sig.address().toString();
	if(nullptr!=c && !mByShortID.contains(id) && !mByHost.contains(address)) {
		mByShortID.insert(id, c);
		mByFullID.insert(fullID, c);
		mByHost.insert(address, c);
		mAll.insert(c);
		emit clientAdded(c);
	} else {
		qWarning()<<"ERROR: could not insert client: "<<c;
	}
}


QSharedPointer<CommsSession> CommsSessionDirectory::getByShortID(const quint64 id)
{
	QMap<quint64, QSharedPointer<CommsSession> >::const_iterator it=mByShortID.find(id);
	if(mByShortID.end()==it) {
		return QSharedPointer<CommsSession>(nullptr);
	}
	return it.value();
}


QSharedPointer<CommsSession> CommsSessionDirectory::getByFullID(const QString &id, const bool addIfMissing)
{
	QMap<QString, QSharedPointer<CommsSession> >::const_iterator it=mByFullID.find(id);
	if(mByFullID.end()==it) {
		if(addIfMissing && mKeyStore.hasPubKeyForID(id)) {
			Key key=mKeyStore.pubKeyForID(id);
			if(key.isValid(true)) {
				QSharedPointer<CommsSession> c(new CommsSession(CommsSignature(id, NetworkAddress()), key));
				insert(c);
				return c;
			}
		}
		return QSharedPointer<CommsSession>(nullptr);
	}
	return it.value();
}
/*

QSharedPointer<CommsSession> CommsSessionDirectory::getByAddress(const NetworkAddress &address, const bool addIfMissing)
{
	QMap<QString, QSharedPointer<CommsSession> >::const_iterator it=mByHost.find(address.toString());
	if(mByHost.end()==it) {
		if(addIfMissing) {
			QSharedPointer<CommsSession> c(new CommsSession(CommsSignature("", address)));
			insert(c);
			return c;
		}
		return QSharedPointer<CommsSession>(nullptr);
	}
	return it.value();
}

*/

QSharedPointer<CommsSession> CommsSessionDirectory::getBySignature(const CommsSignature &signature, const bool addIfMissing)
{
	QSharedPointer<CommsSession> c=QSharedPointer<CommsSession>(nullptr);

	//
	if(nullptr==c) {
//		c=getByAddress(signature.address(), false);
	}
	if(nullptr==c) {
		c=getByShortID(signature.shortHandID());
	}
	if(nullptr==c) {
		c=getByFullID(signature.fullID());
	}
	/*
	if(nullptr==c && addIfMissing) {
		c=QSharedPointer<CommsSession>(new CommsSession(signature));
		insert(c);
	}
	*/
	return c;
}

QSet<QSharedPointer<CommsSession> > CommsSessionDirectory::getByActiveTime(quint64 lastActiveTime)
{
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

int CommsSessionDirectory::count()
{
	return mAll.size();
}


quint32 CommsSessionDirectory::prune()
{
	QSet<QSharedPointer<CommsSession> > remove;
	for(QSharedPointer<CommsSession> client:mAll) {
		if(nullptr!=client) {
			if(client->lastActiveTime()>=client->timeoutAccumulator()) {
				remove << client;
			}
		}
	}
}
