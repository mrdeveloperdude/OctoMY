#include "PortableID.hpp"

#include <QStringList>


PortableID::PortableID()
{


}


bool PortableID::fromString(QString s){
	QStringList parts = s.split(QRegExp ("(\\.)"));
	if(parts.size()!=4){
		return false;
	}
	tier=parts.at(0).trimmed();
	name=parts.at(1).trimmed();
	id=parts.at(2).trimmed();
	pubkey=parts.at(3).trimmed();
	return true;
}

QString PortableID::toString(){
	return tier + "." +name + "." +id+ "." +pubkey;
}
