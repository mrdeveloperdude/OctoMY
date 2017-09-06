#ifndef IDDUEL_HPP
#define IDDUEL_HPP

#include <QString>

class IDDuel
{
private:
	const QString mIDA;
	const QString mIDB;
public:
	IDDuel(QString idA, QString idB);
public:

	bool duel();
	bool duel(QString key);
};

#endif // IDDUEL_HPP
