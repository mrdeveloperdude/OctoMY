#include "ISyncParameter.hpp"

#include "../libutil/utility/Standard.hpp"

QDebug &operator<<(QDebug &d, const ISyncParameter &sp)
{
	return sp.toDebug(d);
}


QDataStream &operator<<(QDataStream &ds, ISyncParameter &sp)
{
	return sp.send(ds);
}

QDataStream &operator>>(QDataStream &ds, ISyncParameter &sp)
{
	return sp.receive(ds);
}





void ISyncParameter::signalValueChanged()
{
	emit mSignaler.syncParameterChanged(this);
}

void ISyncParameter::setHookSignals(QObject &ob, bool hook)
{
	if(hook) {
		if(!QObject::connect(&mSignaler,SIGNAL(syncParameterChanged(ISyncParameter *)),&ob,SLOT(onSyncParameterChanged(ISyncParameter *)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!QObject::disconnect(&mSignaler,SIGNAL(syncParameterChanged(ISyncParameter *)),&ob,SLOT(onSyncParameterChanged(ISyncParameter *)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}
