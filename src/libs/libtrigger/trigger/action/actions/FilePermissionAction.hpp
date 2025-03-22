#ifndef FILEPERMISSIONACTION_HPP
#define FILEPERMISSIONACTION_HPP

#include "trigger/action/Action.hpp"

class ActionContext;

class FilePermissionAction : public ActionBase
{

public:
	FilePermissionAction();
	static QString staticName() { return QStringLiteral("FilePermission"); }
	QString name() const override { return staticName(); }
	bool execute(ActionContext &) override;
};

#endif // FILEPERMISSIONACTION_HPP
