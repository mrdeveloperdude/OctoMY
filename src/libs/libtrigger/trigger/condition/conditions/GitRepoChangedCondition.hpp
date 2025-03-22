#ifndef GITREPOCHANGEDCONDITION_HPP
#define GITREPOCHANGEDCONDITION_HPP

#include "trigger/condition/Condition.hpp"

class GitRepoChangedCondition : public ConditionBase
{
private:
	QString mLastRemoteHead;
public:
	GitRepoChangedCondition();
	static QString staticName() { return QStringLiteral("GitRepoChanged"); }
	QString name() const override{ return staticName(); };
	bool actualEvaluate(ConditionContext &conditionContext) override;
};

#endif // GITREPOCHANGEDCONDITION_HPP
