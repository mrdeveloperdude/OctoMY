#ifndef SERIALSIZE_HPP
#define SERIALSIZE_HPP



#include <QBuffer>
#include <QDataStream>

class SerialSize
{
private:
	QBuffer data;
	QDataStream stream;
public:
	SerialSize()
		: stream(&data)
	{
		data.open(QIODevice::WriteOnly);
	}

	template <typename T>
	quint64 operator ()(const T & t)
	{
		data.seek(0);
		stream << t;
		return data.pos();
	}


};


#endif // SERIALSIZE_HPP
