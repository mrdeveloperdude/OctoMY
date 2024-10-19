#ifndef CONTROLDELIVERYACTIVITY_H
#define CONTROLDELIVERYACTIVITY_H

#include "address/Associate.hpp"
#include "components/navigation/Activity.hpp"
#include "delivery/BirthControl.hpp"
#include "name/RemoteNameGenerator.hpp"
#include "security/PortableID.hpp"

#include <QWidget>
#include <QTimer>


class WaitingSpinnerWidget;

namespace Ui
{
class ControlDeliveryActivity;
}

class Settings;
class Node;
class ControlDeliveryActivity : public Activity
{
	Q_OBJECT

private:
	Ui::ControlDeliveryActivity *ui;
	WaitingSpinnerWidget *mSpinner;
	RemoteNameGenerator mNameGenerator;
	QSharedPointer<Node> mNode;
	PortableID mID;
	QSharedPointer<Associate> mNodeIdentity;
	quint64 mBirthDate;
	BirthControl mBirthControl;
	quint8 mCompleteCounter;
	bool mCompleteOK;
	ConfigureHelper mConfigureHelper;


public:
	static const quint64 MINIMUM_BIRTH_TIME;

public:
	explicit ControlDeliveryActivity(QWidget *parent = nullptr);
	~ControlDeliveryActivity();

public:
	void reset();
	void configure(QSharedPointer<Node> mNode);
	void startBirth(const QString &name);
	
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

#endif // CONTROLDELIVERYACTIVITY_H
