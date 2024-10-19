#ifndef REMOTECONTROLLER_HPP
#define REMOTECONTROLLER_HPP

#include <QWidget>
#include "components/navigation/Activity.hpp"
#include "uptime/ConfigureHelper.hpp"

class Remote;

namespace Ui {
class RemoteController;
}

class RemoteController : public Activity
{
	Q_OBJECT
private:
	Ui::RemoteController *ui;
	QSharedPointer<Remote> mRemote;
	ConfigureHelper mConfigureHelper;
	
public:
	explicit RemoteController(QWidget *parent = nullptr);
	~RemoteController();

public:
	void configure(QSharedPointer<Remote>);
};

#endif // REMOTECONTROLLER_HPP


