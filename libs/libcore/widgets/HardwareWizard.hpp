#ifndef HARDWAREWIZARD_HPP
#define HARDWAREWIZARD_HPP

#include "puppet/PoseMapping.hpp"
#include "hw/actuators/ActuatorControllerFactory.hpp"
#include "puppet/AgentConfigStore.hpp"

#include <QWidget>
#include <QSharedPointer>

class HardwareTemplate;
class HardwareTemplateModel;
class IActuatorController;

namespace Ui
{
class HardwareWizard;
}


class SerialDeviceListModel;
class HardwareWizard : public QWidget
{
	Q_OBJECT

private:

	Ui::HardwareWizard *ui;
	SerialDeviceListModel *mSerialDevicesModel;
	HardwareTemplateModel *mHardwareTemplateModel;
	ActuatorControllerFactory mControllerFactory;
	IActuatorController *mController;

	HardwareTemplate *mSelectedTempalte;
	AgentConfigStore *mConfigStore;

public:
	explicit HardwareWizard(QWidget *parent = 0);
	virtual ~HardwareWizard();

private:
	QString selectedControllerName();
	void moveTo(int next);
	void save();
	void loadFromTemplate();

public:

	void configure(AgentConfigStore &configStore);
	void reset();

signals:

	void done();

private slots:
	void on_pushButtonOnward_clicked();

	void on_pushButtonBack_clicked();


	void on_listWidgetController_doubleClicked(const QModelIndex &index);
	void on_comboBoxAddLocus_currentIndexChanged(int index);
};

#endif // HARDWAREWIZARD_HPP




