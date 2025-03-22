#include "Condition.hpp"

#include "trigger/context/ConditionContext.hpp"
#include "utility/random/Random.hpp"

#include <QElapsedTimer>

ConditionBase::ConditionBase(const QString &phraseText)
	: mLastEvalTime(0)
	, mID{utility::random::randomString(10)}
	, mParameterPhrase(QSharedPointer<ParameterPhrase>::create(phraseText))
{
}

bool ConditionBase::evaluate(ConditionContext &conditionContext)
{
	Q_UNUSED(conditionContext);
	QElapsedTimer timer;
	timer.start();
	bool result = actualEvaluate(conditionContext);
	mLastEvalTime = timer.nsecsElapsed() / 1000; // convert ns to µs
	return result;
}

qint64 ConditionBase::lastEvaluationTime() const
{
	return mLastEvalTime;
}


// Return the ParameterPhrase that encapsulates this condition's description.
QSharedPointer<ParameterPhrase> ConditionBase::phrase() const {
	return mParameterPhrase;
}
