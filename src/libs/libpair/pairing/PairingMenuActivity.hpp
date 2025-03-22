#ifndef PAIRINGMENUACTIVITY_H
#define PAIRINGMENUACTIVITY_H

#include "uptime/ConfigureHelper.hpp"
#include "navigation/MenuActivity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;


/**
 * @brief The PairingMenuActivity class is a menu for the user to select between the
 * different ways of performing pairing. Currently this includes:
 * Network Pairing
 * Camera Pairing
 */

class PairingMenuActivity : public MenuActivity
{
	Q_OBJECT

private:
	ConfigureHelper mConfigureHelper;

public:
	explicit PairingMenuActivity(QWidget *parent = nullptr);
	virtual ~PairingMenuActivity() override;

public:
	void configure(QSharedPointer<Node> node);

private slots:
	void done();
	void networkPairing();
	void cameraPairing();
};

#endif
// PairingActivity_HPP
