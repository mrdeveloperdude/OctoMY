#ifndef CONDITIONFACTORY_HPP
#define CONDITIONFACTORY_HPP

#include "trigger/condition/Condition.hpp"
#include <QString>
#include <QHash>
#include <QSharedPointer>
#include <functional>
#include <type_traits>

class ConditionFactory
{
public:
	using ConditionCreator = std::function<QSharedPointer<ICondition>()>;
	
private:
	QHash<QString, ConditionCreator> mFactoryMap;
	
	ConditionFactory() = default;
	Q_DISABLE_COPY(ConditionFactory)
	
public:
	static ConditionFactory &instance();
	void registerCondition(const QString &key, ConditionCreator creator);
	template <typename T>
	void registerCondition()
	{
		static_assert(std::is_base_of<ICondition, T>::value, "T must derive from ICondition");
		const QString key = T::staticName();
		mFactoryMap[key] = []() -> QSharedPointer<ICondition> {
			return QSharedPointer<T>::create();
		};
	}
	void registerAll();
	QSharedPointer<ICondition> create(const QString &key) const;
	QStringList available() const;
};

#endif // CONDITIONFACTORY_HPP
