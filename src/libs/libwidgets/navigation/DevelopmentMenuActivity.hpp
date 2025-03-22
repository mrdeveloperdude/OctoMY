#ifndef DEVELOPMENTMENUACTIVITY_HPP
#define DEVELOPMENTMENUACTIVITY_HPP

#include "uptime/ConfigureHelper.hpp"
#include "navigation/MenuActivity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;


/**
 * @brief The UtilitiesMenuActivity class collects all development tools in a menu for hub
 */

class DevelopmentMenuActivity : public MenuActivity
{
	Q_OBJECT

private:
	ConfigureHelper mConfigureHelper;

public:
	explicit DevelopmentMenuActivity(QWidget *parent = nullptr);
	virtual ~DevelopmentMenuActivity() override;

public:
	void configure(QSharedPointer<Node> node);
};

#endif
// DEVELOPMENTMENUACTIVITY_HPP
