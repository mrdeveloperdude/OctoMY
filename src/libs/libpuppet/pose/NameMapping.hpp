#ifndef NAMEMAPPING_HPP
#define NAMEMAPPING_HPP

#include <QString>
#include <QMap>



class NameMapping : public QMap<QString, QString>
{
public:
	NameMapping();
	
public:
	QVariantMap toVariantMap() const;
	void fromVariantMap(const QVariantMap &variantMap);
};

#endif // NAMEMAPPING_HPP
