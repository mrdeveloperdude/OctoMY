#include "ConditionFactory.hpp"

ConditionFactory &ConditionFactory::instance()
{
	static ConditionFactory instance;
	return instance;
}

void ConditionFactory::registerCondition(const QString &key, ConditionCreator creator)
{
	mFactoryMap[key] = creator;
}

QSharedPointer<ICondition> ConditionFactory::create(const QString &key) const
{
	if (mFactoryMap.contains(key))
	{
		return mFactoryMap[key]();
	}
	return QSharedPointer<ICondition>(nullptr);
}


QStringList ConditionFactory::available() const{
	return mFactoryMap.keys();
}


#include "trigger/condition/conditions/AlwaysCondition.hpp"
#include "trigger/condition/conditions/ChanceCondition.hpp"
#include "trigger/condition/conditions/GitRepoChangedCondition.hpp"
#include "trigger/condition/conditions/NeverCondition.hpp"

#include "trigger/condition/conditions/CompositeCondition.hpp"

void ConditionFactory::registerAll(){
	registerCondition<AlwaysCondition>();
	registerCondition<ChanceCondition>();
	registerCondition<GitRepoChangedCondition>();
	registerCondition<NeverCondition>();
	registerCondition<CompositeCondition>();
}
