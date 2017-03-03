#ifndef AGENTCONTROLS_HPP
#define AGENTCONTROLS_HPP

class Agent;
class CourierSet;
class CommsSignature;

#include <QHash>

class AgentControls
{
private:

	Agent &mAgent;
	QHash <quint64, CourierSet *> mCouriers;
public:

	explicit AgentControls(Agent &);
	virtual	~AgentControls();

public:
	void registerClient(CommsSignature &sig);
	void unRegisterClient(CommsSignature &sig);

	void setCommsEnabled(bool);

	CourierSet* activeControl() const;
};

#endif // AGENTCONTROLS_HPP
