#ifndef AGENTSTATECOURIER_HPP
#define AGENTSTATECOURIER_HPP

#include "Courier.hpp"
#include "comms/messages/AgentState.hpp"

#include <QObject>


/*!
 * \brief The AgentStateCourier class is responsible for synchronizing an Agent's
 * basic state between controller and agent.
 *
 * The state itself is represented by the AgentState class.
 *
 * It is the Agent's responsibility to persist the state and  perform initial
 * sync upon establishment of new connection
 *
 * Some parameters of the state may be ephemeral, and they may not be stored
 *
 * Most parameters are owned by the controller, meaning that if there is a
 * parameter conflict, the value from the controller will override the value
 * from the agent.
 *
 * Some parameters may be intrinsic meaning there is no overriding authority.
 *
 */
class AgentStateCourier: public Courier
{
	Q_OBJECT
private:
	static const quint32 AGENT_STATE_COURIER_ID;
	bool mAgentSide;
	CourierMandate mMandate;
	AgentState mMessage;
	quint64 mLastSendTime;
	quint64 mLastChangeTime;

	// Agent side constructor
public:
	AgentStateCourier(QDataStream &initialization, QObject *parent = nullptr);


	// Controller side constructor
public:
	AgentStateCourier(QObject *parent = nullptr);

	// Courier interface
public:
	//Let the CommChannel know what we want
	CourierMandate mandate() override;

	//Override to act on sending opportunity.
	//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds) override;

	//Override to act on data received
	//Return number of bytes actually read.
	quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;

public slots:

	void onStatusChanged();

};

#endif // AGENTSTATECOURIER_HPP
