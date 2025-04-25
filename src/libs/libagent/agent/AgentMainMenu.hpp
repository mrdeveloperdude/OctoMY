#ifndef AGENTMAINMENU_HPP
#define AGENTMAINMENU_HPP


#include "uptime/ConfigureHelper.hpp"
#include "navigation/MenuActivity.hpp"

#include <QWidget>
#include <QHostAddress>

class Node;


/**
 * @brief The AgentMainMenu class is the main menu for agent
 */

class AgentMainMenu : public MenuActivity
{
	Q_OBJECT
	
private:
	ConfigureHelper mConfigureHelper;
	QSharedPointer<Node> mNode;
	
public:
	explicit AgentMainMenu(QWidget *parent = nullptr);
	virtual ~AgentMainMenu() override;
	
public:
	void configure(QSharedPointer<Node> node);
	
	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;
};


#endif // AGENTMAINMENU_HPP
