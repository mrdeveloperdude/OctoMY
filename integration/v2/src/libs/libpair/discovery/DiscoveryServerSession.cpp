#include "DiscoveryServerSession.hpp"

#include "uptime/MethodGate.hpp"

#include "DiscoveryServerEntry.hpp"
#include "address/Associate.hpp"

#include <QDebug>

DiscoveryServerSession::DiscoveryServerSession()
{
	OC_METHODGATE();
}


bool DiscoveryServerSession::set(DiscoveryServerEntry part)
{
	OC_METHODGATE();
	if(part.isNull()) {
		qWarning()<<"ERROR: participant was null";
		return false;
	}
	auto id=part.id();
	// Participant allready ni session
	if(has(id)) {
		//qDebug()<<"Participant " << part->ID << "already in session";
		return true;
	}
	// This session is overcrowded, bounce
	if(mParticipantsByID.size()>100)	{
		qWarning()<<"ERROR: Session was full, participant "<<part.id()<<" bounced";
		return false;
	}
	// There is room, insert us
	//qDebug()<<"Session had room for participant "<< part->ID;
	mParticipantsByID[id]=part;
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
	for(DiscoveryServerEntry part:mParticipantsByID) {
		list.append(part.toVariantMap());
	}
	return list;
}


quint64 DiscoveryServerSession::prune(quint64 deadline)
{
	OC_METHODGATE();
	quint64 ct=0;
	for (auto it = mParticipantsByID.begin(), e = mParticipantsByID.end() ; it != e/* not hoisted */; /* no increment */) {
		QString key=it.key();
		DiscoveryServerEntry part=it.value();
		if(part.lastSeen()<=deadline) {
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
	return static_cast<quint64>(mParticipantsByID.size());
}
