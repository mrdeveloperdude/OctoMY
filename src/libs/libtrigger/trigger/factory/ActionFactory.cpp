#include "ActionFactory.hpp"



ActionFactory &ActionFactory::instance()
{
	static ActionFactory instance;
	return instance;
}

void ActionFactory::registerAction(const QString &key, ActionCreator creator)
{
	mFactoryMap[key] = creator;
}

QSharedPointer<IAction> ActionFactory::create(const QString &key) const
{
	if (mFactoryMap.contains(key))
	{
		return mFactoryMap[key]();
	}
	return QSharedPointer<IAction>(nullptr);
}

QStringList ActionFactory::available() const{
	return mFactoryMap.keys();
}


#include "trigger/action/actions/CopyFileAction.hpp"
#include "trigger/action/actions/FilePermissionAction.hpp"
#include "trigger/action/actions/NoOpAction.hpp"
#include "trigger/action/actions/PreserveTriggerAction.hpp"


void ActionFactory::registerAll(){
	registerAction<CopyFileAction>();
	registerAction<FilePermissionAction>();
	registerAction<NoOpAction>();
	registerAction<PreserveTriggerAction>();
}
