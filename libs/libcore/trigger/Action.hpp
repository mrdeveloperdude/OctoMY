#ifndef ACTION_HPP
#define ACTION_HPP

#include <QString>

class ActionContext
{

};

class Action
{
private:
	QString mName;
public:
	Action(QString name="");

	// Action interface
public:
	virtual void perform(ActionContext &);

	virtual QString toString();
};

#endif // ACTION_HPP

