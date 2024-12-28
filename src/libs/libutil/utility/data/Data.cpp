#include "Data.hpp"


#include "uptime/MethodGate.hpp"

#include <QVariantMap>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QDebug>



namespace utility
{
namespace data
{


bool mapIsIn(const QVariantMap &orig, const QVariantMap &in, const bool careAboutType)
{
	OC_FUNCTIONGATE();
	for(QVariantMap::const_iterator i = in.constBegin(), e=in.constEnd(); e != i; ++i) {
		const auto k=i.key();
		//const auto v=i.value();
		if(!orig.contains(k)) {
			qWarning().noquote().nospace()<<"MISMATCH: Orig did not contain key "<<k<< " (which btw should have a value of '"<< in[k]<<"' )";
			return false;
		}
		if(careAboutType && (orig[k].typeId() != in[k].typeId()) ) {
			qWarning().noquote().nospace()<<"MISMATCH: Type for key "<<k<< " was '"<< orig[k].typeId()<<"' but should be '"<< in[k].typeId()<<"' )";
			return false;
		}
		if (in[k].typeId() == QMetaType::QVariantMap) {
			const bool ok=mapIsIn(orig[k].toMap(), in[k].toMap(), careAboutType);
			if(!ok) {
				return false;
			}
		} else if(orig[k].toString() != in[k].toString()) {
			qWarning().noquote().nospace()<<"MISMATCH: Value for key "<<k<< " was '"<< orig[k].toString()<<"' but should be '"<< in[k].toString()<<"' )";
			return false;
		}
	}
	return true;
}


void merge( QVariantMap  &c, QVariantMap  &b)
{
	OC_FUNCTIONGATE();
	//QVariantMap  c(a);
	for(QVariantMap::const_iterator i=b.begin(), e=b.end() ; i!=e ; ++i) {
		c.insert(i.key(), i.value());
	}
	//return c;
}

}
}
