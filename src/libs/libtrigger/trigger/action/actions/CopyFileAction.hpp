#ifndef COPYFILEACTION_HPP
#define COPYFILEACTION_HPP

#include "trigger/action/Action.hpp"

class ActionContext;

// Sample action that copies a file. Expects parameters "source" and "destination".
class CopyFileAction : public ActionBase
{
public:
	CopyFileAction();
	static QString staticName() { return QStringLiteral("CopyFile"); }
	QString name() const override{ return staticName(); };
	bool execute(ActionContext &) override;
};

#endif // COPYFILEACTION_HPP
