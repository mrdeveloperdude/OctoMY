#include "DiscoveryServer.hpp"
#include <QCryptographicHash>

#include <QRegularExpression>
#include <QDebug>



DiscoveryServerSession * DiscoveryServer::request(QSharedPointer<DiscoveryParticipant> part)
{
	if(nullptr==part){
		qWarning()<<"ERROR: participant was 0";
		return nullptr;
	}
	if(!part->isValidForServer()){
		qWarning()<<"ERROR: participant was invalid: "<<part->toString();
		return nullptr;
	}
	DiscoveryServerSession *ses=nullptr;
	if(!part->pins.isEmpty()){
		QString pin;
		for(QString p: part->pins){
			pin=p;
			//Look up the pin in question
			if(registry.contains(pin)){
				//qDebug()<<"PIN "<<pin<<" matched with a session";
				ses=registry[pin];
				break;
			}
		}
		if(""!=pin){
			if(nullptr==ses){
				//qDebug()<<"Creating new session";
				ses=new DiscoveryServerSession();
				if(!ses->set(part)){
					qWarning()<<"ERROR: Newly created session will be terminated as participant was not welcome";
					delete ses;
					ses=nullptr;
				}
				else{
					//qDebug()<<"Adding session to registry under pin "<<pin;
					registry[pin]=ses;
				}
			}
			else if(!ses->set(part)){
				qWarning()<<"ERROR: participant was not welcome in existing session";
				ses=nullptr;
			}
		}
		else{
			qWarning()<<"ERROR: no pins";
		}
	}
	else{
		qWarning()<<"ERROR: participant had no pins";
	}
	return ses;
}




void DiscoveryServer::prune(quint64 deadline)
{
	quint64 participantPruneCount=0;
	quint64 sessionPruneCount=0;
	for(QMap<QString, DiscoveryServerSession *>::iterator it=registry.begin();it!=registry.end() /* not hoisted */; /* no increment */ ){
		QString key=it.key();
		DiscoveryServerSession *ses=it.value();
		if(nullptr!=ses){
			const quint64 tmp=ses->prune(deadline);
			if(tmp>0){
				participantPruneCount+=tmp;
				if(ses->count()<=0){
					delete ses;
					ses=nullptr;
					registry.erase(it++);
					sessionPruneCount++;
					continue;
				}
			}
		}
		++it;
	}
	if(participantPruneCount>0){
		qDebug()<<"PRUNING "<<participantPruneCount<<" participants from server";
	}
	if(sessionPruneCount>0){
		qDebug()<<"PRUNING "<<sessionPruneCount<<" sessions from server";
	}
}
