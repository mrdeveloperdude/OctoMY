#ifndef HARDWAREWIZARD_HPP
#define HARDWAREWIZARD_HPP

#include "puppet/PoseMapping.hpp"
#include <QWidget>
#include <QSharedPointer>

class HardwareTemplate;
class HardwareTemplateModel;


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

	HardwareTemplate *mSelectedTempalte;
	QSharedPointer<PoseMapping> mPoseMapping;

public:
	explicit HardwareWizard(QWidget *parent = 0);
	virtual ~HardwareWizard();

private:
	void moveTo(int next);
	void save();
	void loadFromTemplate();

public:

	void configure(QSharedPointer<PoseMapping>);
	void reset();

signals:

	void done();

private slots:
	void on_pushButtonOnward_clicked();

	void on_pushButtonBack_clicked();


};

#endif // HARDWAREWIZARD_HPP
