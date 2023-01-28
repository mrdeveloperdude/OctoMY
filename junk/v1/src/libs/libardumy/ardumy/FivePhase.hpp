#ifndef FIVEPHASE_HPP
#define FIVEPHASE_HPP

#include <inttypes.h>
#include <stddef.h>

enum FIVE_PHASE_TABLE_TYPE {
	SINGLE_STEP
	, DOUBLE_STEP
	, MICRO_STEP
};

class FivePhase
{
public:
	FivePhase();

public:
	template <typename T>
	void generateTable(T *ret, size_t stepCount, FIVE_PHASE_TABLE_TYPE type, size_t phaseCount, T phaseShift, T offset, T stateCount, T startState);

	template <typename T>
	void bob(T *ret, size_t stepCount, size_t phaseCount, T phaseShift, T offset, T stateCount, T startState);


};


template <typename T>
void FivePhase::generateTable(T *ret, const size_t stepCount, const FIVE_PHASE_TABLE_TYPE type, const size_t phaseCount, const T phaseShift,const T offset, const T stateCount, const T startState)
{

	switch(type) {
	case(SINGLE_STEP): {
		bob(ret, stepCount, phaseCount, phaseShift, offset, stateCount, startState);
	}
	break;
	case(DOUBLE_STEP): {

	}
	break;
	case(MICRO_STEP): {


	}
	break;
	}

}



template <typename T>
void FivePhase::bob(T *ret, const size_t stepCount,  const size_t phaseCount, const T phaseShift, const T offset, const T stateCount, const T startState)
{
	T state=startState;
	const T train=1;
	T beg_a=0;
	T end_a=beg_a+train;
	for(T i=0; i< stepCount; ++i) {
		for(T j=beg_a; j<end_a; ++j) {
			T jj=j%phaseCount;
			ret[i] |= ((1<<((jj + phaseShift) % phaseCount)))<<offset;
		}
		if(stateCount-2==state) {
			end_a++;
		} else if(stateCount-1==state) {
			beg_a++;
		}
		state=(state+1)%stateCount;
	}
}



#endif // FIVEPHASE_HPP
