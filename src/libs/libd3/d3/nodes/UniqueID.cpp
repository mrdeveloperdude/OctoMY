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
