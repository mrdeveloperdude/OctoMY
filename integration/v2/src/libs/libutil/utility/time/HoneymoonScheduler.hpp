#ifndef HONEYMOONTIMER_HPP
#define HONEYMOONTIMER_HPP

#include "uptime/ConfigureHelper.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"

#include <QDateTime>

/**
 * @brief The HoneymoonScheduler class is used to calculate the schedule when to perform certain actions.
 * It is called honeymoon because it resembles the imaginary love curve for newly wed couples. It is
 * very intense in the start then drops gradually off into obscurity, but might spike at later points
 * before dropping off again.
 *
 * Parameter:
 */


template <typename T>
class HoneymoonScheduler
{
protected:
	// Parameters
	T mTriggeredOutput;
	T mIdleOutput;
	quint64 mGracePeriod;
	quint64 mDecayPeriod;

	// State
	quint64 mLastTrigger;

	ConfigureHelper mConfigureHelper;


public:
	explicit HoneymoonScheduler();
	virtual ~HoneymoonScheduler();

public:
	/**
	 * @brief configure
	 * @param maxOutput - highest output value at full output
	 * @param gracePeriod - length of full output period after last trigger
	 * @param decayPeriod - length of decay period after grace runs out
	 * @param minOutput - lowest output value after decay
	 *
	 */
	void configure(T maxOutput, quint64 gracePeriod, quint64 decayPeriod, T minOutput);

	/**
	 * @brief trigger -  trigger a new honeymoon
	 * @param now - time now in millis since unix epoch if you have it or 0 to let us figure it out ourselves
	 */
	void trigger(quint64 now=0);

	/**
	 * @brief reset - decay completely
	 * @param now - time now in millis since unix epoch if you have it or 0 to let us figure it out ourselves
	 */
	void reset(quint64 now=0);

	/**
	 * @brief currentValue - return the current value
	 * @param now - time now in millis since unix epoch if you have it or 0 to let us figure it out ourselves
	 * @return the value as calcualted from the time since last honeymoon and internla parameters for max output, grace period, decay period and min output.
	 */
	T currentValue(quint64 now=0);
};

template <typename T>
HoneymoonScheduler<T>::HoneymoonScheduler()
// Default to sane values with a freshly triggered honeymoon
	: mTriggeredOutput(1)
	, mIdleOutput(0)
	, mGracePeriod(10000) // 10 seconds
	, mDecayPeriod(20000) // 20 seconds
	, mLastTrigger(utility::time::currentMsecsSinceEpoch<quint64>())
	, mConfigureHelper("HoneymoonScheduler", true, false, true, true, true)
{
	OC_METHODGATE();

}

template <typename T>
HoneymoonScheduler<T>::~HoneymoonScheduler()
{
	OC_METHODGATE();
}



template <typename T>
void HoneymoonScheduler<T>::configure(T maxOutput, quint64 gracePeriod, quint64 decayPeriod, T minOutput)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mTriggeredOutput=maxOutput;
		mGracePeriod=gracePeriod;
		mDecayPeriod=decayPeriod;
		mIdleOutput=minOutput;
	}
}


template <typename T>
void HoneymoonScheduler<T>::trigger(quint64 now)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(0==now) {
			now=utility::time::currentMsecsSinceEpoch<quint64>();
		}
		// Set last trigger time to now to enable grace period
		mLastTrigger=now;
	}
}


template <typename T>
void HoneymoonScheduler<T>::reset(quint64 now)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(0==now) {
			now=utility::time::currentMsecsSinceEpoch<quint64>();
		}
		// Simply skip past the honeymoon by moving last trigger beyond the grace and decay period combined
		mLastTrigger=now - (mGracePeriod + mDecayPeriod + 1);
	}
}


template <typename T>
T HoneymoonScheduler<T>::currentValue(quint64 now)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(0==now) {
			now=utility::time::currentMsecsSinceEpoch<quint64>();
		}
		// Last trigger is in the future, preten everything is fine
		if(mLastTrigger>=now) {
			return mTriggeredOutput;
		}
		// Calcualte time since last trigger
		quint64 iv=now-mLastTrigger;
		// We are in the honeymoon phase
		if(iv<mGracePeriod) {
			return mTriggeredOutput;
		}
		// No skip past grace period
		iv-=mGracePeriod;
		// We are past the decay period
		if(iv>mDecayPeriod) {
			return mIdleOutput;
		}
		// We are in the decay period, so calculate linear decay value
		//iv=mDecayPeriod-iv;
		const T range=(mIdleOutput-mTriggeredOutput);
		T ret=mTriggeredOutput;
		const T dp=static_cast<T>(mDecayPeriod);
		T t=(iv * range);
		t/=dp;
		ret += t;
		return ret;
	}
	return 0;
}


#endif
// HONEYMOONTIMER_HPP

