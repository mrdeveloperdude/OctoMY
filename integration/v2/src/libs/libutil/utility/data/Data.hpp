#ifndef DATA_HPP
#define DATA_HPP

#include <QVariantMap>

namespace utility
{
namespace data
{

//Data
bool mapIsIn(const QVariantMap &orig, const QVariantMap &in, const bool careAboutType=false);
void merge( QVariantMap  &c, QVariantMap  &b);


template <typename K, typename V>
QVector<QPair<K, V> > convert(QMap<K, V> map);






////////////////////////////////////////////////////////////////////////////////


template <typename K, typename V>
QVector<QPair<K, V> > convert(QMap<K, V> map)
{
	QVector<QPair<K, V> > out(map.size());
	int index=0;
	for(auto end=map.constEnd(), it=map.constBegin(); it!=end; ++it) {
		out[index++]=QPair<K, V>(it.key(), it.value());
	}

	return out;
}




}
}

#endif
// DATA_HPP
