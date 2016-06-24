#include "DiscoveryServerSession.hpp"

#include "DiscoveryParticipant.hpp"

#include <QDebug>

DiscoveryServerSession::DiscoveryServerSession()
{

}


bool DiscoveryServerSession::set(QSharedPointer<DiscoveryParticipant> part){
	if(nullptr==part){
		qWarning()<<"ERROR: participant was 0";
		return false;
	}
	// Allready there
	if(participants.contains(part->ID)){
		//qDebug()<<"Participant " << part->ID << "already in session";
		return true;
	}
	// This session is overcrowded, bounce
	if(participants.size()>100)	{
		qWarning()<<"ERROR: Session was full, participant "<<part->ID<<" bounced";
		return false;
	}
	// There is room, insert us
	//qDebug()<<"Session had room for participant "<< part->ID;
	participants[part->ID]=part;
	return true;
}


QVariantList DiscoveryServerSession::toVariantMap(){
	QVariantList list;
	for(QSharedPointer<DiscoveryParticipant> part:participants){
		list.append(part->toVariantMap());
	}
	return list;
}
