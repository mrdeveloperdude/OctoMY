#include "DiscoveryServerSession.hpp"

#include "basic/NodeAssociate.hpp"

#include <QDebug>

DiscoveryServerSession::DiscoveryServerSession()
{

}


bool DiscoveryServerSession::set(QSharedPointer<NodeAssociate> part){
	if(nullptr==part){
		qWarning()<<"ERROR: participant was 0";
		return false;
	}
	// Allready there
	if(mParticipantsByID.contains(part->id())){
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

bool DiscoveryServerSession::has(QString id){
	return mParticipantsByID.contains(id);
}


QVariantList DiscoveryServerSession::toVariantMap(){
	QVariantList list;
	for(QSharedPointer<NodeAssociate> part:mParticipantsByID){
		list.append(part->toVariantMap());
	}
	return list;
}


quint64 DiscoveryServerSession::prune(quint64 deadline)
{
	quint64 ct=0;
	for (QMap<QString, QSharedPointer<NodeAssociate>>::iterator  it = mParticipantsByID.begin(); it != mParticipantsByID.end() /* not hoisted */; /* no increment */){
		QString key=it.key();
		QSharedPointer<NodeAssociate> part=it.value();
		if(part->lastSeen()<=deadline){
			mParticipantsByID.erase(it++);
			ct++;
		}
		else{
			++it;
		}
	}
	return ct;
}


quint64 DiscoveryServerSession::count()
{
	return mParticipantsByID.size();
}
