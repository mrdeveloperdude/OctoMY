#ifndef CONTROLLERACTIVITY_HPP
#define CONTROLLERACTIVITY_HPP

#include <QWidget>

#include "components/navigation/Activity.hpp"

#include "uptime/ConfigureHelper.hpp"

class Agent;

namespace Ui {
class ControllerActivity;
}

/**
 * @brief The ControllerActivity class is an activity to show the settings of a controller to the user.
 * The reason why controller configs are not separate activities is because they are part of a plugin
 * architecture. Each controller may be located in s separate executable module such as .dll including 
 * the UI for configuring it. Thus the controller will simply provide a pointer to a QWidget instasnce
 * instead of bothering with registering and unregistering of an Activity.
 * 
 * The widget is placed inside a scrollable area so that it can occupy as much space as it likes without
 * being cropped.
 */

class ControllerActivity : public Activity
{
	Q_OBJECT
private:
	Ui::ControllerActivity *ui;
	QSharedPointer<Agent> mAgent;
	bool mDebug{false};
	ConfigureHelper mConfigureHelper;
	

public:
	explicit ControllerActivity(QWidget *parent = nullptr);
	~ControllerActivity();

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;

public:
	void configure(QSharedPointer<Agent> agent);
	void updateController();

private slots:
	void confirmConfiguration();

};

#endif // CONTROLLERACTIVITY_H
