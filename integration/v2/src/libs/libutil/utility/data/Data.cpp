#include "Data.hpp"


#include "uptime/MethodGate.hpp"

#include <QVariantMap>
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
		if(careAboutType && (orig[k].type() != in[k].type()) ) {
			qWarning().noquote().nospace()<<"MISMATCH: Type for key "<<k<< " was '"<< orig[k].type()<<"' but should be '"<< in[k].type()<<"' )";
			return false;
		}
		if(in[k].type()==QVariant::Map) {
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
