#include "LoopingBuffer.hpp"

LoopingBuffer::LoopingBuffer(QByteArray buf)
	: mBuf(buf)
{

}

int LoopingBuffer::actualIndex(int i)const
{
	const auto sz=mBuf.size();
	if(sz<0) {
		return -1;
	}
	if(i<0) {
		return -1;
	}
	return i % sz;
}

char LoopingBuffer::at(int i) const
{
	auto ai=actualIndex(i);
	if(ai<0) {
		return 0;
	}
	return mBuf[ai];
}

char LoopingBuffer::operator[](int i) const
{
	return at(i);
}


void LoopingBuffer::doXor(QByteArray &ba) const
{
	auto sz=ba.size();
	for(int i=0;i<sz;++i){
		ba[i]=ba[i]^at(i);
	}
}
