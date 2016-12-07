#include "ISyncParameter.hpp"


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
