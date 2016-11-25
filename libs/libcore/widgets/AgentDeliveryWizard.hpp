#ifndef AGENTDELIVERYWIZARD_HPP
#define AGENTDELIVERYWIZARD_HPP

#include "security/KeyStore.hpp"
#include "security/PortableID.hpp"
#include "basic/AgentNameGenerator.hpp"
#include "basic/NodeAssociate.hpp"


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
	Settings *mSettings;
	Node *mNode;
	PortableID mID;
	QSharedPointer<NodeAssociate> mMyData;
	quint64 mBirthDate;

	QMutex timeoutMutex;
	quint8 completeCounter;
	bool completeOK;

public:

	static const quint64 MINIMUM_BIRTH_TIME;

public:
	explicit AgentDeliveryWizard(QWidget *parent = 0);
	~AgentDeliveryWizard();


public:
	void reset();

	void configure(Node *mNode);

	void startBirth();

signals:
	void done(bool);

private slots:
	void onBirthComplete(bool);

private slots:
	void on_pushButtonDone_clicked();
	void on_pushButtonPairNow_clicked();
	void on_pushButtonRandomName_clicked();
	void on_pushButtonOnward_clicked();
	void on_pushButtonRandomGender_clicked();

};

#endif // AGENTDELIVERYWIZARD_HPP
