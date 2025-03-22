#ifndef CONDITION_HPP
#define CONDITION_HPP

#include "LogicalOp.hpp"
#include "trigger/phrase/ParameterPhrase.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QVariantMap>
#include <QElapsedTimer>
#include <QList>

class ConditionContext;

// ICondition interface.
class ICondition
{
public:
	virtual ~ICondition() = default;
	virtual QString name() const = 0;
	virtual QString id() const =0;
	virtual bool evaluate(ConditionContext &) = 0;
	// Returns the last evaluation time in microseconds.
	virtual qint64 lastEvaluationTime() const = 0;
	
	virtual QSharedPointer<ParameterPhrase> phrase() const = 0;
};


// Base class for conditions with timing evaluation.
class ConditionBase : public ICondition
{
protected:
	qint64 mLastEvalTime; // in microseconds
	QString mID;
	QSharedPointer<ParameterPhrase> mParameterPhrase;

public:
	ConditionBase(const QString &phraseText);
	virtual ~ConditionBase() = default;
	QString id() const override {return mID; };
	bool evaluate(ConditionContext &) override;
	virtual bool actualEvaluate(ConditionContext &conditionContext) = 0;
	qint64 lastEvaluationTime() const override;
	
	// Return the ParameterPhrase that encapsulates this condition's description.
	QSharedPointer<ParameterPhrase> phrase() const override;
};

#endif // CONDITION_HPP
