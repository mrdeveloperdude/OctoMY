#ifndef IDENTITY_ACTIVITY_HPP
#define IDENTITY_ACTIVITY_HPP

#include "uptime/ConfigureHelper.hpp"
#include "navigation/MenuActivity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;


/**
 * @brief The IdentityMenuActivity class is a menu for the user to select between 
 * available identity management screens
 */

class IdentityMenuActivity : public MenuActivity
{
	Q_OBJECT

private:
	ConfigureHelper mConfigureHelper;

public:
	explicit IdentityMenuActivity(QWidget *parent = nullptr);
	virtual ~IdentityMenuActivity() override;

public:
	void configure(QSharedPointer<Node> node);

private slots:
	void birthCertificate();
	void abortion();
	void delivery();
	void done();
};

#endif
// IDENTITY_ACTIVITY_HPP
