#include "ISyncParameter.hpp"

#include "uptime/MethodGate.hpp"

#include "uptime/ConnectionType.hpp"


QDebug &operator<<(QDebug &d, const ISyncParameter &sp)
{
	OC_FUNCTIONGATE();
	return sp.toDebug(d);
}


QDataStream &operator<<(QDataStream &ds, ISyncParameter &sp)
{
	OC_FUNCTIONGATE();
	return sp.send(ds);
}

QDataStream &operator>>(QDataStream &ds, ISyncParameter &sp)
{
	OC_FUNCTIONGATE();
	return sp.receive(ds);
}





void ISyncParameter::signalValueChanged()
{
	OC_METHODGATE();
	emit mSignaler.syncParameterChanged(this);
}

void ISyncParameter::setHookSignals(QObject &ob, bool hook)
{
	OC_FUNCTIONGATE();
	if(hook) {
		if(!QObject::connect(&mSignaler,SIGNAL(syncParameterChanged(ISyncParameter *)),&ob,SLOT(onSyncParameterChanged(ISyncParameter *)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!QObject::disconnect(&mSignaler,SIGNAL(syncParameterChanged(ISyncParameter *)),&ob,SLOT(onSyncParameterChanged(ISyncParameter *)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}
