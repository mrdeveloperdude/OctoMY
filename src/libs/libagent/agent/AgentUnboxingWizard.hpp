#ifndef AGENTUNBOXINGWIZARD_HPP
#define AGENTUNBOXINGWIZARD_HPP

#include <QWidget>

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "UnboxingStage.hpp"
#include "components/navigation/Activity.hpp"


class Agent;
class ControllerHandler;
class IController;

namespace Ui {
class AgentUnboxingWizard;
}

class AgentUnboxingWizard : public Activity
{
	Q_OBJECT

private:
	Ui::AgentUnboxingWizard *ui;
	QSharedPointer<Agent> mAgent;
	ConfigureHelper mConfigureHelper;

public:
	explicit AgentUnboxingWizard(QWidget *parent = nullptr);
	~AgentUnboxingWizard();
	
private:
	QString controllerName() const;
	QSharedPointer<ControllerHandler> controllerHandler() const;
	QSharedPointer<IController> controller() const;
	bool isDelivered() const;
	bool isControllerSet() const;
	bool isControllerConfigured() const;
	int stanzaCount() const;
	bool isStanzaed() const;
	int associateCount() const;
	bool isPaired() const;
	bool isPairingSkipped() const;
	bool controllerNeedsConfig() const;

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;

public:
	UnboxingStage unboxingStage();
	bool unboxingDone();
	void configure(QSharedPointer<Agent> agent);
	
public slots:
	void updateStage();
	void nextStageClicked();

};

#endif // AGENTUNBOXINGWIZARD_HPP
