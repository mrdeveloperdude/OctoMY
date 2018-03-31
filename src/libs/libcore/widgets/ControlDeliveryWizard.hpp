#ifndef CONTROLDELIVERYWIZARD_HPP
#define CONTROLDELIVERYWIZARD_HPP

#include "security/KeyStore.hpp"
#include "security/PortableID.hpp"
#include "basic/Associate.hpp"


#include <QWidget>
#include <QTimer>


class WaitingSpinnerWidget;

namespace Ui
{
class ControlDeliveryWizard;
}

class Settings;
class Node;
class ControlDeliveryWizard : public QWidget
{
	Q_OBJECT

private:
	Ui::ControlDeliveryWizard *ui;
	QTimer mBirthTimer;
	WaitingSpinnerWidget *mSpinner;
	Settings *mSettings;
	QSharedPointer<Node> mNode;
	PortableID mID;
	QSharedPointer<Associate> mNodeIdentity;
	quint64 mBirthDate;

	quint8 mCompleteCounter;
	bool mCompleteOK;


public:
	static const quint64 MINIMUM_BIRTH_TIME;

public:
	explicit ControlDeliveryWizard(QWidget *parent = 0);
	~ControlDeliveryWizard();

public:
	void reset();
	void configure(QSharedPointer<Node> mNode);
	void startBirth();

signals:
	void done(bool);

private slots:
	void onBirthComplete(bool ok);

private slots:
	void on_pushButtonPairNow_clicked();

};

#endif // CONTROLDELIVERYWIZARD_HPP
