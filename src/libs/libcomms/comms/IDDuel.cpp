#include "IDDuel.hpp"

#include <QCryptographicHash>
#include <QByteArray>

IDDuel::IDDuel(QString idA, QString idB)
	: mIDA(idA)
	, mIDB(idB)
{

}


bool IDDuel::duel(QString key)
{
		   QCryptographicHash hashA(QCryptographicHash::Sha1);
		   hashA.addData(key+mIDA);
		   QByteArray rawA=hashA.result();
		   QCryptographicHash hashB(QCryptographicHash::Sha1);
		   hashB.addData(key+mIDB);
		   QByteArray rawB=hashB.result();
		   return rawA>rawB;
}
