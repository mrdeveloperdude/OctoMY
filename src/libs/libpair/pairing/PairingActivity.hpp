#ifndef PAIRINGACTIVITY_H
#define PAIRINGACTIVITY_H

#include "uptime/ConfigureHelper.hpp"
#include "components/navigation/Activity.hpp"

#include <QWidget>
#include <QHostAddress>

namespace Ui
{
class PairingActivity;
}

class Node;


/**
 * @brief The PairingActivity class is a menu for the user to select between the
 * different ways of performing pairing. Currently this includes:
 * Network Pairing
 * Camera Pairing
 */

class PairingActivity : public Activity
{
	Q_OBJECT

private:
	Ui::PairingActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;


public:
	explicit PairingActivity(QWidget *parent = nullptr);
	virtual ~PairingActivity() override;

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
