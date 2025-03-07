#ifndef CAMERAPAIRINGACTIVITY_H
#define CAMERAPAIRINGACTIVITY_H

#include <QWidget>

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Agent;

namespace Ui {
class CameraPairingActivity;
}

class CameraPairingActivity : public Activity
{
	Q_OBJECT
private:
	Ui::CameraPairingActivity *ui;
	QSharedPointer<Agent> mAgent;

public:
	explicit CameraPairingActivity(QWidget *parent = nullptr);
	~CameraPairingActivity();

public:
	void configure(QSharedPointer<Agent> agent);

};

#endif // CAMERAPAIRINGACTIVITY_H
