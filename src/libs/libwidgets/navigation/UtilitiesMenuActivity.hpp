#ifndef UTILITIESMENUACTIVITY_HPP
#define UTILITIESMENUACTIVITY_HPP

#include "uptime/ConfigureHelper.hpp"
#include "navigation/MenuActivity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;


/**
 * @brief The UtilitiesMenuActivity class collects all utilities in a menu for hub
 */

class UtilitiesMenuActivity : public MenuActivity
{
	Q_OBJECT

private:
	ConfigureHelper mConfigureHelper;

public:
	explicit UtilitiesMenuActivity(QWidget *parent = nullptr);
	virtual ~UtilitiesMenuActivity() override;

public:
	void configure(QSharedPointer<Node> node);
};

#endif
// UTILITIESMENUACTIVITY_HPP
