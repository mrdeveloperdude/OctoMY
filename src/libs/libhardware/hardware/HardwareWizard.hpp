#ifndef HARDWAREWIZARD_HPP
#define HARDWAREWIZARD_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>


class HardwareTemplate;
class HardwareTemplateModel;
class IActuatorController;
class AgentConfigStore;
class Agent;

namespace Ui
{
class HardwareWizard;
}



class HardwareWizard : public QWidget
{
	Q_OBJECT

private:
	Ui::HardwareWizard *ui;
	HardwareTemplate *mSelectedTempalte;
	QSharedPointer<Agent> mAgent;
	HardwareTemplateModel *mHardwareTemplateModel;

public:
	explicit HardwareWizard(QWidget *parent = nullptr);
	virtual ~HardwareWizard();

private slots:
	void on_pushButtonOnward_clicked();
	void on_pushButtonBack_clicked();
	void on_listWidgetController_doubleClicked(const QModelIndex &index);

};

#endif
// HARDWAREWIZARD_HPP
