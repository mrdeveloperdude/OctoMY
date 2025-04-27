#include "Layer.hpp"

QDebug operator<<(QDebug dbg, const Layer &node)
{
	QDebugStateSaver saver(dbg);
	dbg.nospace() << node.toString();
	return dbg;
}
