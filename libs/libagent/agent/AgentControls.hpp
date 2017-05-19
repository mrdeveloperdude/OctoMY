#ifndef AGENTCONTROLS_HPP
#define AGENTCONTROLS_HPP

class Agent;
class CourierSet;


#include <QHash>

class AgentControls
{
private:

	Agent &mAgent;
	QHash <QString , CourierSet *> mCouriers;
public:

	explicit AgentControls(Agent &);
	virtual	~AgentControls();

public:
	void registerClient(QString fullID);
	void unRegisterClient(QString fullID);

	void setCommsEnabled(bool);

	CourierSet* activeControl() const;
};

#endif // AGENTCONTROLS_HPP
