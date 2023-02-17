#ifndef HARDWAREWIZARD_HPP
#define HARDWAREWIZARD_HPP

#include "pose/PoseMapping.hpp"
#include "hardware/controllers/ActuatorControllerFactory.hpp"

#ifdef OC_USE_FEATURE_MOTORICS
#include "motorics/MtlobeController.hpp"
#endif

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


struct ControllerStanza {
	QString nickName;
	QString fullName;
	QString iconURL;

	ControllerStanza(
		QString nickName
		, QString fullName
		, QString iconURL
	)
		: nickName (nickName)
		, fullName (fullName)
		, iconURL (iconURL)
	{

	}
};

struct ControllerStanzaList: public QList<ControllerStanza> {

};


struct MtlobeStanza {
	QString nickName;
	QString fullName;
	QString iconURL;

	MtlobeStanza(
		QString nickName
		, QString fullName
		, QString iconURL
	)
		: nickName (nickName)
		, fullName (fullName)
		, iconURL (iconURL)
	{

	}
};

struct MtlobeStanzaList: public QList<MtlobeStanza> {

};


class SerialDeviceListModel;
class HardwareWizard : public QWidget
{
	Q_OBJECT

private:

	Ui::HardwareWizard *ui;
	SerialDeviceListModel *mSerialDevicesModel;
	HardwareTemplateModel *mHardwareTemplateModel;
	HardwareTemplate *mSelectedTempalte;
	QSharedPointer<Agent> mAgent;
	ControllerStanzaList controllerStanzas;
	MtlobeStanzaList mMtlobeStanzas;

#ifdef OC_USE_FEATURE_MOTORICS
	MtlobeController mMtlobeController;
	MtlobeManagerWidget *mWidgetMotorLobeManager;
#endif

public:
	explicit HardwareWizard(QWidget *parent = nullptr);
	virtual ~HardwareWizard();

private:
	void setupMtLobeUi();
	void initControllerList();
	void initMtlobeList();
	int controllerIndexByName(QString name);
	QString selectedControllerName();
	void moveTo(int next);
	void save();
	void loadFromTemplate();

public:
	void configure(QSharedPointer<Agent> agent);
	void reset();

signals:
	void done();

private slots:
	void on_pushButtonOnward_clicked();
	void on_pushButtonBack_clicked();
	void on_listWidgetController_doubleClicked(const QModelIndex &index);
	void on_comboBoxAddMtlobe_currentIndexChanged(int index);
};

#endif
// HARDWAREWIZARD_HPP
