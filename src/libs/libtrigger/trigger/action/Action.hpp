#ifndef ACTION_HPP
#define ACTION_HPP

#include "trigger/phrase/ParameterPhrase.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QVariantMap>

class ActionContext;

// IAction interface.
class IAction
{
public:
	virtual ~IAction() = default;
	virtual QString name() const = 0;
	virtual QString id() const =0;
	virtual bool execute(ActionContext &) = 0;
	virtual QSharedPointer<ParameterPhrase> phrase() = 0;

};


// Base class for actions.
class ActionBase : public IAction
{
private:
	QString mID;
	QSharedPointer<ParameterPhrase> mPhrase;
public:
	ActionBase(const QString &phraseText);
	virtual ~ActionBase() = default;
	QString id() const override {return mID; };
	QSharedPointer<ParameterPhrase> phrase() override {
		return mPhrase;
	}

};



#endif // ACTION_HPP
