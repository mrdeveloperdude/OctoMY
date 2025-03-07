#ifndef PAIRINGMENUACTIVITY_H
#define PAIRINGMENUACTIVITY_H

#include "uptime/ConfigureHelper.hpp"
#include "components/navigation/Activity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;

namespace Ui
{
class PairingMenuActivity;
}

/**
 * @brief The PairingMenuActivity class is a menu for the user to select between the
 * different ways of performing pairing. Currently this includes:
 * Network Pairing
 * Camera Pairing
 */

class PairingMenuActivity : public Activity
{
	Q_OBJECT

private:
	Ui::PairingMenuActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;


public:
	explicit PairingMenuActivity(QWidget *parent = nullptr);
	virtual ~PairingMenuActivity() override;

public:
	void configure(QSharedPointer<Node> node);
	
	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;

private slots:
	void done();
	void networkPairing();
	void cameraPairing();
};

#endif
// PairingActivity_HPP
