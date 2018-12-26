#include "TestUtility.hpp"

#include "utility/random/Random.hpp"

// This test is very pragmatic. It basically checks to see that prng built into Qt returns
// somewhat sane outputs.

void TestUtility::testRandom()
{
	const float smallF=0.01f;
	const float bigF=1.0f-smallF;
	const float midF=0.5f;
	const double smallD=0.01;
	const double bigD=1.0-smallD;
	const double midD=0.5;
	float minF=2.0f;
	float maxF=-2.0f;
	float sumF=0.0f;
	double minD=2.0;
	double maxD=-2.0;
	double sumD=0.0;
	const int num=10000;
	for(int i=0; i<num; ++i) {
		const auto f=utility::random::frand();
		const auto d=utility::random::dfrand();
		sumF+=f;
		sumD+=d;
		if(f>maxF) {
			maxF=f;
		}
		if(f<minF) {
			minF=f;
		}
		if(d>maxD) {
			maxD=d;
		}
		if(d<minD) {
			minD=d;
		}
	}
	const double avgD=sumD/num;
	const float avgF=sumF/num;
	QVERIFY(minF < smallF);
	QVERIFY(minD < smallD);
	QVERIFY(maxF > bigF);
	QVERIFY(maxD > bigD);
	QVERIFY(avgF < (midF+smallF/2.0f));
	QVERIFY(avgF > (midF-smallF/2.0f));
	QVERIFY(avgD < (midD+smallD/2.0));
	QVERIFY(avgD > (midD-smallD/2.0));
}
