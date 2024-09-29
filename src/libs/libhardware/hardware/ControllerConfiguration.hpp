#ifndef CONTROLLERCONFIGURATION_HPP
#define CONTROLLERCONFIGURATION_HPP

#include <QWidget>

#include "components/navigation/Activity.hpp"

class Agent;

namespace Ui {
class ControllerConfiguration;
}

class ControllerConfiguration : public Activity
{
	Q_OBJECT
private:
	Ui::ControllerConfiguration *ui;
	QSharedPointer<Agent> mAgent;

public:
	explicit ControllerConfiguration(QWidget *parent = nullptr);
	~ControllerConfiguration();

public:
	void configure(QSharedPointer<Agent> agent);
	void reset();
	void init();

signals:
	void done();

private slots:
	void confirmConfiguration();
};

#endif // CONTROLLERCONFIGURATION_HPP
