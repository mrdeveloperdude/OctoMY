#ifndef AGENTCONTROLS_HPP
#define AGENTCONTROLS_HPP

class Agent;
class AgentCourierSet;


#include <QHash>

class AgentControls
{
private:

	Agent &mAgent;
	QHash <QString , AgentCourierSet *> mCouriers;
public:

	explicit AgentControls(Agent &);
	virtual	~AgentControls();

public:
	void registerClient(QString fullID);
	void unRegisterClient(QString fullID);

	void setCommsEnabled(bool);

	AgentCourierSet* activeControl() const;
};

#endif // AGENTCONTROLS_HPP
