#ifndef IDDUEL_HPP
#define IDDUEL_HPP

#include <QString>

class IDDuel
{
private:
	QString mIDA;
	QString mIDB;
public:
	IDDuel(QString idA, QString idB);
public:

	bool duel(QString key);
};

#endif // IDDUEL_HPP
