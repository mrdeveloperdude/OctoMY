#ifndef HARDWAREACTIVITY_HPP
#define HARDWAREACTIVITY_HPP

#include <QWidget>

#include "components/navigation/Activity.hpp"

#include "uptime/ConfigureHelper.hpp"

class Agent;

namespace Ui {
class HardwareActivity;
}

/**
 * @brief The HardwareActivity class is an activity to show alternatives for managing
 * agent hardware. There are many screens to chose from, so it works as sort of a
 * top-level menu for hardware management.
 */

class HardwareActivity : public Activity
{
	Q_OBJECT
private:
	Ui::HardwareActivity *ui;
	QSharedPointer<Agent> mAgent;
	bool mDebug{false};
	ConfigureHelper mConfigureHelper;

public:
	explicit HardwareActivity(QWidget *parent = nullptr);
	~HardwareActivity();

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity,
				 const QStringList returnArguments = QStringList()) override;

public:
	void configure(QSharedPointer<Agent> agent);

private slots:
	void actuators();
	void controller();
	void done();
	void lobes();
	void sensors();
	void stanzas();
};

#endif // HARDWAREACTIVITY_HPP
