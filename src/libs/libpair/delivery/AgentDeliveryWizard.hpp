#ifndef AGENTDELIVERYWIZARD_HPP
#define AGENTDELIVERYWIZARD_HPP

#include "security/KeyStore.hpp"
#include "security/PortableID.hpp"
#include "name/AgentNameGenerator.hpp"
#include "address/Associate.hpp"

#include <QWidget>
#include <QTimer>


class WaitingSpinnerWidget;

namespace Ui
{
class AgentDeliveryWizard;
}

class Settings;
class Node;
class AgentDeliveryWizard : public QWidget
{
	Q_OBJECT

protected:
	Ui::AgentDeliveryWizard *ui;
	QTimer mBirthTimer;
	WaitingSpinnerWidget *mSpinner;
	AgentNameGenerator mNameGenerator;
	QSharedPointer<Node> mNode;
	PortableID mID;
	QSharedPointer<Associate> mNodeIdentity;
	quint64 mBirthDate;

	QMutex mTimeoutMutex;
	quint8 mCompleteCounter;
	bool mCompleteOK;

public:
	static const quint64 MINIMUM_BIRTH_TIME;

public:
	explicit AgentDeliveryWizard(QWidget *parent = nullptr);
	virtual ~AgentDeliveryWizard();

public:
	void configure(QSharedPointer<Node> mNode);
	void reset();
	void startBirth();

private:
	Settings *settings();


signals:
	void done(bool);

private slots:
	void onBirthComplete(bool);

private slots:
	void on_pushButtonPairNow_clicked();
	void on_pushButtonRandomName_clicked();
	void on_pushButtonOnward_clicked();
	void on_pushButtonRandomGender_clicked();

};

#endif
// AGENTDELIVERYWIZARD_HPP
