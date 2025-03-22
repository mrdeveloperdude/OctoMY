#ifndef ACTIONFACTORY_HPP
#define ACTIONFACTORY_HPP


#include "trigger/action/Action.hpp"

#include <QString>
#include <QHash>
#include <QSharedPointer>
#include <functional>
#include <type_traits>
	
	class ActionFactory
{
public:
	using ActionCreator = std::function<QSharedPointer<IAction>()>;
	
private:
	QHash<QString, ActionCreator> mFactoryMap;
	
	ActionFactory() = default;
	Q_DISABLE_COPY(ActionFactory)
	
public:
	static ActionFactory &instance();
	
	void registerAction(const QString &key, ActionCreator creator);
	
	template <typename T>
	void registerAction()
	{
		static_assert(std::is_base_of<IAction, T>::value, "T must derive from IAction");
		const QString key = T::staticName();
		mFactoryMap[key] = []() -> QSharedPointer<IAction> {
			return QSharedPointer<T>::create();
		};
	}

	void registerAll();
	QSharedPointer<IAction> create(const QString &key) const;
	QStringList available() const;
};


#endif // ACTIONFACTORY_HPP
