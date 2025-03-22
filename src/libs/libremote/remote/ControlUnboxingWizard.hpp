#ifndef CONTROLUNBOXINGWIZARD_HPP
#define CONTROLUNBOXINGWIZARD_HPP

#include "components/navigation/Activity.hpp"
#include "remote/ControlUnboxingStage.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QWidget>

class Node;

namespace Ui {
class ControlUnboxingWizard;
}

class ControlUnboxingWizard : public Activity
{
	Q_OBJECT
private:
	Ui::ControlUnboxingWizard *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	
public:
	explicit ControlUnboxingWizard(QWidget *parent = nullptr);
	~ControlUnboxingWizard();

private:
	bool isDelivered() const;
	bool isPaired() const;
	bool isPairingSkipped() const;

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;

public:
	ControlUnboxingStage unboxingStage();
	bool unboxingDone();
	void configure(QSharedPointer<Node> node);
	
public slots:
	void updateStage();
	void nextStageClicked();

};

#endif // CONTROLUNBOXINGWIZARD_HPP
