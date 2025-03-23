#include "utility/random/Random.hpp"

#include <QString>
#include <QtMath>
#include <QRandomGenerator>


/*


SEE ALSO: librng/random/ * 
*/
namespace utility
{

namespace random
{


quint32 qrand()
{
	return QRandomGenerator::global()->generate();
}

void qsrand(quint32 s){
	QRandomGenerator::global()->seed(s);
}

float frand()
{
	float v=qrand();
	v/=static_cast<float>(0xFFFFFFFF);
	return v;
}

double dfrand()
{
	double v=qrand();
	v/=RAND_MAX;
	return v;
}

static int ipow(int base, int exp)
{
	int result = 1;
	while (exp) {
		if (exp & 1) {
			result *= base;
		}
		exp >>= 1;
		base *= base;
	}

	return result;
}

QByteArray randomByteArray(int size)
{
	QByteArray out;
	int bpc=(ipow(2,sizeof(char)*8));
	for(int i=0; i<size; ++i) {
		const char ch=static_cast<char>(qrand() % bpc);
		out.append(ch);
	}
	return out;
}


QString randomString(int size, int base)
{
	static const QString alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"};
	if (base > alphabet.size())
		base = alphabet.size();
	QString out;
	for (int i = 0; i < size; ++i)
	{
		int index = QRandomGenerator::global()->bounded(base);
		out.append(alphabet.at(index));
	}
	return out;
}


}

}

