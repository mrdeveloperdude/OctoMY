#ifndef HUB_MAIN_MENU_HPP
#define HUB_MAIN_MENU_HPP

#include "uptime/ConfigureHelper.hpp"
#include "navigation/MenuActivity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;


/**
 * @brief The HubMainMenu class is the main menu for hub
 */

class HubMainMenu : public MenuActivity
{
	Q_OBJECT

private:
	ConfigureHelper mConfigureHelper;

public:
	explicit HubMainMenu(QWidget *parent = nullptr);
	virtual ~HubMainMenu() override;

public:
	void configure(QSharedPointer<Node> node);
};

#endif
// HUB_MAIN_MENU_HPP
