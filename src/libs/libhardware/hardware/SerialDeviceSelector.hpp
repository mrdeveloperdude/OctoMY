#ifndef SERIALDEVICESELECTOR_H
#define SERIALDEVICESELECTOR_H

#include <QWidget>

#include "components/navigation/Activity.hpp"

class SerialDeviceListModel;
class HardwareTemplateModel;
class Agent;




namespace Ui {
class SerialDeviceSelector;
}

class SerialDeviceSelector : public Activity
{
	Q_OBJECT
private:
	Ui::SerialDeviceSelector *ui;
	QSharedPointer<Agent> mAgent;
	SerialDeviceListModel *mSerialDevicesModel{nullptr};

public:
	explicit SerialDeviceSelector(QWidget *parent = nullptr);
	~SerialDeviceSelector();

public:
	void configure(QSharedPointer<Agent> agent);
	void reset();

signals:
	void done(const QString &controller);
	
private slots:
	void selectController();


};

#endif // SERIALDEVICESELECTOR_H
