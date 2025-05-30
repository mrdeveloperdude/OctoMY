/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef AGENTSTATECOURIER_HPP
#define AGENTSTATECOURIER_HPP

#include "Courier.hpp"

#include "comms/sync/SyncContext.hpp"
#include "comms/messages/AgentMode.hpp"


#include <QObject>
#include <QDebug>

#include <QGeoCoordinate>

template <typename T>
class SyncParameter;

class QGeoCoordinate;
class Pose;

/*!
 * \brief The AgentStateCourier class is responsible for synchronizing an Agent's
 * basic state between controller and agent.
 *
 * The state itself is represented by an instance of SyncContext with parameters
 * for agent flags, agent mode, target position, target orientation and target pose
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
	//AgentState mMessage;
	SyncContext mParams;
	quint64 mLastSendTime;
	quint64 mLastChangeTime;

	// Parameters
	SyncParameter<quint8> *mFlags; // The boolean flags part of the state. The following are the values:
	// BIT-0 EmergencyShutdown: ASIMOV: When true this robot is completely shut down. It can be triggered automatically when faults are found (to uphold Asimov's first law) or manually either by remote or local.
	// BIT-1 CameraEnabled: When true this robot will keep camera running and broadcast frames via BlobCourier. Enabled by remote.
	// BIT-2 Reserved for future use
	// BIT-3 Reserved for future use
	// BIT-4 Reserved for future use
	// BIT-5 Reserved for future use
	// BIT-6 Reserved for future use
	// BIT-7 Reserved for future use

	SyncParameter<AgentMode> *mMode; // The desired mode of operation for this robot
	SyncParameter<QGeoCoordinate> *mTargetPosition; // The desired target position for the robot in lat,long,alt. The robot may or may not actively pursue this location depending on the mode it is in.
	SyncParameter<qreal> *mTargetOrientation; // The desired target orientation for the robot in radians starting from North. The robot may or may not actively pursue this orientation depending on the mode it is in.
	SyncParameter<Pose> *mTargetPose; // The desired target pose for the robot in actuator positions. The robot may or may not actively pursue this pose depending on the mode it is in.



//TODO: Somehow encode desired speed, direction and more

public:
	explicit AgentStateCourier(QDataStream *initialization, QSharedPointer<Comms> comms, QObject *parent = nullptr);
	virtual	~AgentStateCourier() override;

public:




	// Parameter access
public:
	void setPanic(bool);
	void setFlags(quint8 flags);
	void setMode(AgentMode mode);
	void setPoseValue(quint32 index, qreal value);


	bool panic();


	// Logging and debugging
public:
	QString toString() const;
	QDebug &toDebug(QDebug &d) const;



private:
	void initParams(QDataStream *initialization);

public:
	// Signal hooks
	void setHookSignals(QObject &ob, bool hook);


	// Courier interface
public:

	// Update courier state when channel has opportunity
	void update(quint64 now) override;

	// Let the CommChannel know what we want
	CourierMandate mandate() const override;

	// Override to act on sending opportunity.
	// Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds) override;

	// Override to act on data received
	// Return number of bytes actually read.
	quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;


};


const QDebug &operator<<(QDebug &d, const AgentStateCourier &);


#endif
// AGENTSTATECOURIER_HPP
