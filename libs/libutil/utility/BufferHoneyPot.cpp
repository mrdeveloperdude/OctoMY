#include "BufferHoneyPot.hpp"

BufferHoneyPot::BufferHoneyPot(bool doScopedReport):
	buf{0}
	,doScope(doScopedReport)
	,seed(0)
	,mwc_z(0)
	,mwc_w(0)
{
	prepare();
}

BufferHoneyPot::~BufferHoneyPot()
{
	size_t ct=compare();
	if(doScope && ct>0) {
		qWarning()<<"ERROR: buffer honeypot reported "<< QString::number(ct)<<" changes signifying possible invalid memory access";
	}
}


void BufferHoneyPot::prepare()
{
	seed=(qint64)this;
	mwc_reseed(seed);
	for(size_t i=0; i<SIZE; i++) {
		buf[i]=(char)(mwc()%0xff);
	}
}

qint64 BufferHoneyPot::compare()
{
	mwc_reseed(seed);
	size_t ct=0;
	size_t rct=0;
	for(size_t i=0; i<SIZE; i++) {
		const char a=(char)(mwc()%0xff);
		const char b=buf[i];
		if(a!=b) {
			ct++;
			rct++;
			qDebug()<<"A="<<a<<" B="<<b;
		} else {
			rct=0;
		}
	}
	return ct;
}

void BufferHoneyPot::report()
{
	size_t ct=compare();
	if(ct>0) {
		qWarning()<<"ERROR: buffer honeypot reported "<< QString::number(ct)<<" changes signifying possible invalid memory access";
	}
}

char *BufferHoneyPot::getData()
{
	return buf;
}
