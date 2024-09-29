#ifndef AGENTUNBOXINGWIZARD_HPP
#define AGENTUNBOXINGWIZARD_HPP

#include <QWidget>

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "UnboxingStage.hpp"
#include "components/navigation/Activity.hpp"


class Agent;

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
	bool isDelivered() const;
	bool isControllerSet() const;
	bool isControllerConfigured() const;
	bool isPaierd() const;
	bool controllerIsSerial() const;
	bool controllerNeedsConfig() const;

public:
	UnboxingStage unboxingStage();
	bool unboxingDone();
	void configure(QSharedPointer<Agent> agent);
	
public slots:
	void updateStage();
	void nextStageClicked();

signals:
	void nextUnboxingStage(UnboxingStage stage);
	void unboxingStageChanged(UnboxingStage stage);
	
};

#endif // AGENTUNBOXINGWIZARD_HPP
