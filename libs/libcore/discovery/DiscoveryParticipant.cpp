#include "QSharedPointer<NodeAssociate>.hpp"
#include <QDateTime>

#include <QRegularExpression>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>

/*


//////////////////////////////////////////////////////////////////////////

QSharedPointer<NodeAssociate>::QSharedPointer<NodeAssociate>()
	: isDeleted(false)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
}



QSharedPointer<NodeAssociate>::QSharedPointer<NodeAssociate>(QVariantMap map)
	: assoc(map, true)
	, isDeleted(false)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
}





QSharedPointer<NodeAssociate>::QSharedPointer<NodeAssociate>(const QSharedPointer<NodeAssociate> &o)
	: assoc(o.assoc)
	, pins(o.pins)
	, isDeleted(false)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
}


QSharedPointer<NodeAssociate>::~QSharedPointer<NodeAssociate>()
{
	isDeleted=true;
}


bool QSharedPointer<NodeAssociate>::operator==(const QSharedPointer<NodeAssociate> &o) const
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return assoc == o.assoc;
}

bool QSharedPointer<NodeAssociate>::isValidForServer()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return (!pins.isEmpty()) && assoc.isValid() ;
}


bool QSharedPointer<NodeAssociate>::isValidForClient()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return assoc.isValid();
}


QVariantMap QSharedPointer<NodeAssociate>::toVariantMap()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	QVariantMap map=assoc.toVariantMap();
	map["pins"]=pins;
	return map;
}



QString QSharedPointer<NodeAssociate>::toString()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return assoc.toString()+", pins:"+pins.join(";");
}

const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


void QSharedPointer<NodeAssociate>::clearPins()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	pins.clear();
}


void QSharedPointer<NodeAssociate>::addPin(QString pin)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	if(rePin.match(pin).hasMatch()){
		//qDebug()<<"ACCEPTED PIN:" <<pin;
		pins<<pin;
	}
	else {
		//qDebug()<<"Pin "<<pin<<" did not match validation:";
	}
}





void QSharedPointer<NodeAssociate>::clearTrust()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	assoc.clearTrust();
}

void QSharedPointer<NodeAssociate>::addTrust(QString trust)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	assoc.addTrust(trust);
}


ClientSignature QSharedPointer<NodeAssociate>::clientSignature()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return ClientSignature(assoc.id(), assoc.publicAddress());
}

bool QSharedPointer<NodeAssociate>::updateFromServer(QVariantMap &map, bool trustedSource)
{
	return assoc.update(map, trustedSource);
}




const QDebug &operator<<(QDebug &d, QSharedPointer<NodeAssociate> &part)
{
	OC_FUNCTIONGATE();
	Q_ASSERT(!part.isDeleted);
	d.nospace() << "QSharedPointer<NodeAssociate>( assoc="<<part.toString()<<")";
	return d.maybeSpace();
}



const QDebug &operator<<(QDebug &d, QSharedPointer<NodeAssociate> &part)
{
	OC_FUNCTIONGATE();
	Q_ASSERT(!part.isDeleted);
	d.nospace() << "QSharedPointer<NodeAssociate>( assoc="<<part.toString()<<")";
	return d.maybeSpace();
}



*/
