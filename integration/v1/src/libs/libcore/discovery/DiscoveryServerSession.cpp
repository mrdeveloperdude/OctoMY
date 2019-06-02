#include "DiscoveryServerSession.hpp"

#include "basic/Associate.hpp"

#include <QDebug>

DiscoveryServerSession::DiscoveryServerSession()
{
	OC_METHODGATE();
}


bool DiscoveryServerSession::set(QSharedPointer<Associate> part)
{
	OC_METHODGATE();
	if(part.isNull()) {
		qWarning()<<"ERROR: participant was 0";
		return false;
	}
	// Allready there
	if(has(part->id())) {
		//qDebug()<<"Participant " << part->ID << "already in session";
		return true;
	}
	// This session is overcrowded, bounce
	if(mParticipantsByID.size()>100)	{
		qWarning()<<"ERROR: Session was full, participant "<<part->id()<<" bounced";
		return false;
	}
	// There is room, insert us
	//qDebug()<<"Session had room for participant "<< part->ID;
	mParticipantsByID[part->id()]=part;
	return true;
}

bool DiscoveryServerSession::has(QString id)
{
	OC_METHODGATE();
	return mParticipantsByID.contains(id);
}


QVariantList DiscoveryServerSession::toVariantMap()
{
	OC_METHODGATE();
	QVariantList list;
	for(QSharedPointer<Associate> part:mParticipantsByID) {
		list.append(part->toVariantMap());
	}
	return list;
}


quint64 DiscoveryServerSession::prune(quint64 deadline)
{
	OC_METHODGATE();
	quint64 ct=0;
	for (QMap<QString, QSharedPointer<Associate>>::iterator  it = mParticipantsByID.begin(); it != mParticipantsByID.end() /* not hoisted */; /* no increment */) {
		QString key=it.key();
		QSharedPointer<Associate> part=it.value();
		if(part->lastSeen()<=deadline) {
			mParticipantsByID.erase(it++);
			ct++;
		} else {
			++it;
		}
	}
	return ct;
}


quint64 DiscoveryServerSession::count()
{
	OC_METHODGATE();
	return mParticipantsByID.size();
}
