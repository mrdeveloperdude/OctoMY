#ifndef CLIENTLIST_HPP
#define CLIENTLIST_HPP

#include "Client.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QList>
#include <QMap>

class AddressBook;
class CommsSessionDirectory;
class Node;
/**
 * @brief The ClientList class is a convenience wrapper around QList to keep pointers to instances of Client
 */

class ClientList
{
private:
	QList<QSharedPointer<Client> > mClients;

public:
	explicit ClientList();
	virtual ~ClientList();

public:

	QMap<QString, QSharedPointer<Client> > toMapByID();
	void syncToAddressBook(QSharedPointer<AddressBook> ab, QSharedPointer<Node> node);


	QSharedPointer<Client> byID(QString id);
	QSet<QSharedPointer<Client> > withActiveSessions(CommsSessionDirectory &sessionDirectory, AddressBook &ab, quint64 now=0, const bool honeyMoon=false);

	void clear();

	QList < QSharedPointer<ClientWidget> > widgets();

	int count() const;

	// [Un]register the couriers of all clients in this list
	void registerAllCouriers(const bool reg);

	// Update the registrations of all clients in this list looking at their individual "needsConnection()" output
	void updateCourierRegistration();
};

#endif
// CLIENTLIST_HPP

// TODO: What is this?
/*

#include "AgentControls.hpp"



AgentControls mControls;

, mControls(*this)

	qDebug()<<"AGENT found in addressbook "<< (ready?"READY":"UNREADY");
	QMap<QString, QSharedPointer<Associate> > &associates = mAddressBook.all();
	for(QSharedPointer<Associate> associate:associates) {
		QSharedPointer<AddressEntry> ae=associate->addressList().highestScore();
		if(!ae.isNull() && ae->address.isValid() && ROLE_CONTROL == associate->role()) {
			qDebug()<<"Adding "<<ae->address.toString()<<"("<<associate->toString()<<") to agent control's list";
			mControls.registerClient(associate->id());
		}
	}
	mControls.setCommsEnabled(true);


	const AgentControls &controls() const ;

const AgentControls &Agent::controls() const
{
	OC_METHODGATE();
	return mControls;
}


void Agent::setPanic(bool panic)
{
	OC_METHODGATE();
	AgentCourierSet *set=mControls.activeControl();

	if(nullptr!=set) {
		QSharedPointer<AgentStateCourier> asc=set->agentStateCourier();
		if(!asc.isNull()) {
			asc->setPanic(panic);
		} else {
			qWarning()<<"ERROR: No active control";
		}
	} else {
		qWarning()<<"ERROR: No set";
	}

}



		// Adaptively start commschannel when there are couriers registered
		const int ct = cc->courierCount();
		if(ct > 0) {
			if(nullptr != assoc) {
				startComms();
			}
		} else {
			stopComms();
		}



void FaceWidget::setAgent(QSharedPointer<Agent> a):
		const AgentControls &ctl=mAgent->controls();
		AgentCourierSet *set = ctl.activeControl();
		if(nullptr!=set) {
			QSharedPointer<AgentStateCourier> asc=set->agentStateCourier();
			if(!asc.isNull()) {
				asc->setHookSignals(*this,true);
			}

		}



void AgentWindow::onSyncParameterChanged(ISyncParameter *sp):
	const AgentControls &controls=mAgent->controls();
	AgentCourierSet *set = controls.activeControl();
	if(nullptr!=set) {
		QSharedPointer<AgentStateCourier> asc=set->agentStateCourier();
		if(!asc.isNull()) {
			const bool panic=asc->panic();
			ui->widgetFace->setPanic(panic);
			ui->widgetFace->panic();
		}
	}
*/
