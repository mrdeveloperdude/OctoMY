#ifndef REMOTEMAINMENU_HPP
#define REMOTEMAINMENU_HPP


#include "uptime/ConfigureHelper.hpp"
#include "navigation/MenuActivity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;


/**
 * @brief The RemoteMainMenu class is the main menu for Remote
 */

class RemoteMainMenu : public MenuActivity
{
	Q_OBJECT
	
private:
	ConfigureHelper mConfigureHelper;
	
public:
	explicit RemoteMainMenu(QWidget *parent = nullptr);
	virtual ~RemoteMainMenu() override;
	
public:
	void configure(QSharedPointer<Node> node);
};


#endif // REMOTEMAINMENU_HPP
