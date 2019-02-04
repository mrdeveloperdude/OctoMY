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

}
}

#endif
// DATA_HPP
