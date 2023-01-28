#ifndef BUFFERHONEYPOT_HPP
#define BUFFERHONEYPOT_HPP

#include <QtCore>


class BufferHoneyPot
{
private:
	const static size_t SIZE=20;
	char buf[SIZE];
	bool doScope;
	qint64 seed;
	qint64	mwc_z = 362436069;
	qint64 mwc_w = 521288629;


public:
	explicit BufferHoneyPot(bool doScopedReport=false);
	virtual ~BufferHoneyPot();
public:
	void prepare();
	qint64 compare();
	void report();
	char *getData();

private:
	inline qint64 mwc(void)
	{
		mwc_z = 36969 * (mwc_z & 65535) + (mwc_z >> 16);
		mwc_w = 18000 * (mwc_w & 65535) + (mwc_w >> 16);
		return (mwc_z << 16) + mwc_w;
	}
	inline void mwc_reseed(qint64 s)
	{
		seed=s;
		mwc_z = 362436069^seed;
		mwc_w = 521288629;
	}
};


#endif // BUFFERHONEYPOT_HPP
