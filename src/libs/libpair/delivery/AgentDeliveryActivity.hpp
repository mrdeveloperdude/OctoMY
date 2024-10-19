#ifndef AGENTDELIVERYACTIVITY_HPP
#define AGENTDELIVERYACTIVITY_HPP

#include "address/Associate.hpp"
#include "components/navigation/Activity.hpp"
#include "delivery/BirthControl.hpp"


#include <QWidget>

class WaitingSpinnerWidget;

namespace Ui
{
class AgentDeliveryActivity;
}

class Settings;
class Node;
class PortableID;
class AgentDeliveryActivity : public Activity
{
	Q_OBJECT

private:
	Ui::AgentDeliveryActivity *ui;
	WaitingSpinnerWidget *mSpinner;
	AgentNameGenerator mNameGenerator;
	QSharedPointer<Node> mNode;
	quint64 mBirthDate;
	BirthControl mBirthControl;
	quint8 mCompleteCounter;
	bool mCompleteOK;
	bool mLastBootstrap{false};
	ConfigureHelper mConfigureHelper;

public:
	explicit AgentDeliveryActivity(QWidget *parent = nullptr);
	virtual ~AgentDeliveryActivity();

public:
	void configure(QSharedPointer<Node> mNode);
	void reset();
	void startBirth(const QString &name, const QString &gender);

private:
	void configureSpinner();

private slots:
	void onBirthProgress(const QString &step, int index, int total);
	void onBirthComplete(bool success, const QString &message);

	void deliveryDone();
	
	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;
};

#endif
// AGENTDELIVERYACTIVITY_HPP
