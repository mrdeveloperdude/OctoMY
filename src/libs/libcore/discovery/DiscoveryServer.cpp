#include "DiscoveryServer.hpp"
#include <QCryptographicHash>

#include <QRegularExpression>
#include <QDebug>



DiscoveryServerSession * DiscoveryServer::request(QSharedPointer<Associate> part)
{
	OC_METHODGATE();
	quint32 participantAddCount=0;
	quint32 sessionAddCount=0;
	if(nullptr==part){
		qWarning()<<"ERROR: participant was nullptr";
		return nullptr;
	}
	if(!part->isValidForServer()){
		qWarning()<<"ERROR: participant was invalid: "<<part->toString();
		return nullptr;
	}
	DiscoveryServerSession *ses=nullptr;
	const QStringList &pins=part->pins();
	if(!pins.isEmpty()){
		QString pin;
		for(QString p: pins){
			pin=p;
			//Look up the pin in question
			if(mRegistry.contains(pin)){
				//qDebug()<<"PIN "<<pin<<" matched with a session";
				ses=mRegistry[pin];
				break;
			}
		}
		if(""!=pin){
			if(nullptr==ses){
				//qDebug()<<"Creating new session";
				ses=OC_NEW DiscoveryServerSession();
				if(!ses->set(part)){
					qWarning()<<"ERROR: Newly created session will be terminated as participant was not welcome";
					delete ses;
					ses=nullptr;
				}
				else{
					//qDebug()<<"Adding session to registry under pin "<<pin;
					mRegistry[pin]=ses;
					sessionAddCount++;
					participantAddCount++;
				}
			}
			else {
				if(!ses->has(part->id())){
					if(!ses->set(part)){
						qWarning()<<"ERROR: participant was not welcome in existing session";
						ses=nullptr;
					}
					else{
						participantAddCount++;
					}

				}
			}
		}
		else{
			qWarning()<<"ERROR: no pin";
		}
	}
	else{
		qWarning()<<"ERROR: participant had no pins";
	}
	if(sessionAddCount>0){
		qDebug()<<"ADDING "<<sessionAddCount<<" sessions to server";
	}
	if(participantAddCount>0){
		qDebug()<<"ADDING "<<participantAddCount<<" participants to server";
	}
	return ses;
}




void DiscoveryServer::prune(quint64 deadline)
{
	OC_METHODGATE();
	quint64 participantPruneCount=0;
	quint64 sessionPruneCount=0;
	for(QMap<QString, DiscoveryServerSession *>::iterator it=mRegistry.begin();it!=mRegistry.end() /* not hoisted */; /* no increment */ ){
		QString key=it.key();
		DiscoveryServerSession *ses=it.value();
		if(nullptr!=ses){
			const quint64 tmp=ses->prune(deadline);
			if(tmp>0){
				participantPruneCount+=tmp;
				if(ses->count()<=0){
					delete ses;
					ses=nullptr;
					mRegistry.erase(it++);
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


QVariantList DiscoveryServer::toVariantList()
{
	OC_METHODGATE();
	QVariantList out;
	for(QMap<QString, DiscoveryServerSession *>::iterator it=mRegistry.begin();it!=mRegistry.end() /* not hoisted */; /* no increment */ ){
		QString key=it.key();
		DiscoveryServerSession *ses=it.value();
		if(nullptr!=ses){
			QVariantList asses=ses->toVariantMap();
			out << asses;
		}
		++it;
	}
	return out;
}
