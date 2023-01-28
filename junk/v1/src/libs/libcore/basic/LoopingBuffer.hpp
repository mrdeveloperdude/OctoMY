#ifndef LOOPINGBUFFER_HPP
#define LOOPINGBUFFER_HPP

#include <QByteArray>
class LoopingBuffer
{
private:
	QByteArray mBuf;
public:
	LoopingBuffer(QByteArray buf);

	int actualIndex(int i)const;
	char at(int i) const;
	char operator[](int i) const;
	void doXor(QByteArray &ba) const;
};

#endif // LOOPINGBUFFER_HPP
