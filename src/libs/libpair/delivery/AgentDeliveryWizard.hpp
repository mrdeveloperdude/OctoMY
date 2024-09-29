#ifndef AGENTDELIVERYWIZARD_HPP
#define AGENTDELIVERYWIZARD_HPP

#include "address/Associate.hpp"
#include "components/navigation/Activity.hpp"
#include "delivery/BirthControl.hpp"
#include "name/AgentNameGenerator.hpp"

#include <QWidget>

class WaitingSpinnerWidget;

namespace Ui
{
class AgentDeliveryWizard;
}

class Settings;
class Node;
class PortableID;
class AgentDeliveryWizard : public Activity
{
	Q_OBJECT

private:
	Ui::AgentDeliveryWizard *ui;
	ConfigureHelper mConfigureHelper;
	WaitingSpinnerWidget *mSpinner;
	AgentNameGenerator mNameGenerator;
	QSharedPointer<Node> mNode;
	quint64 mBirthDate;
	BirthControl mBirthControl;
	quint8 mCompleteCounter;
	bool mCompleteOK;
	bool mUseVoice{false};
	bool mLastBootstrap{false};

public:
	explicit AgentDeliveryWizard(QWidget *parent = nullptr);
	virtual ~AgentDeliveryWizard();

public:
	void configure(QSharedPointer<Node> mNode);
	void reset();
	void startBirth();

private:
	void configureSpinner();
	void configureValidators();
	Settings *settings();
	void announceBirth(const PortableID &id);

signals:
	void done(bool);

private slots:
	void onBirthProgress(const QString &step, int index, int total);
	void onBirthComplete(bool success, const QString &message);

private slots:
	void randomizeName();
	void randomizeGender();
	void startDelivery();
	void deliveryDone();

};

#endif
// AGENTDELIVERYWIZARD_HPP
