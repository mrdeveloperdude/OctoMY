#include "UniqueID.hpp"

#include <QRandomGenerator>

QString generateUniqueID() {
	static const QString UNIQUE_ID_ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static const int ID_LENGTH = 20;
	QString uniqueID;
	uniqueID.reserve(ID_LENGTH);
	for (int i = 0; i < ID_LENGTH; ++i) {
		int index = QRandomGenerator::global()->bounded(UNIQUE_ID_ALPHABET.size());
		uniqueID.append(UNIQUE_ID_ALPHABET.at(index));
	}
	return uniqueID;
}

static int idCounter=0;

UniqueID::UniqueID()
	: mUniqueID(generateUniqueID())
	, mID(idCounter++)
{
}


QDataStream& operator<<(QDataStream& out, const UniqueID& obj) {
	out << obj.mUniqueID;
	return out;
}


QDataStream& operator>>(QDataStream& in, UniqueID& obj) {
	in >> obj.mUniqueID;
	return in;
}
