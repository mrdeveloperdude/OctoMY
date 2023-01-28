#ifndef INCHWORMIO_HPP
#define INCHWORMIO_HPP

#include "audio/utils/Inchworm.hpp"

#include <QIODevice>


class InchwormIO: public QIODevice{
	Q_OBJECT
public:
	static constexpr size_t SIZE=64;
	typedef Inchworm<SIZE, qint16 > Inchtype;
private:
	Inchtype mBuffer;
	
public:
	InchwormIO(){}
	
public:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 maxlen) override;
	
signals:
	// Notify that we have a need for more samples on the write end
	void samplesNeeded(InchwormIO::Inchtype &buf);
	// Notify that we have more samples on the read end
	void samplesAvailable(InchwormIO::Inchtype &buf);
	
};



#endif // INCHWORMIO_HPP
