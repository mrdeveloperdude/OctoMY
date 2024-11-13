#ifndef CONTROLLERTYPESELECTOR_HPP
#define CONTROLLERTYPESELECTOR_HPP

#include <QWidget>

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Agent;
class ControllerTypeModel;

namespace Ui {
class ControllerTypeSelector;
}

class ControllerTypeSelector : public Activity
{
	Q_OBJECT
private:
	Ui::ControllerTypeSelector *ui;
	QSharedPointer<Agent> mAgent;
	ControllerTypeModel *mControllerTypeModel{nullptr};

public:
	explicit ControllerTypeSelector(QWidget *parent = nullptr);
	~ControllerTypeSelector();

private:
	void updateList();

public:
	void configure(QSharedPointer<Agent> agent);
	void reset();

private slots:
	void selectController();

};

#endif // CONTROLLERTYPESELECTOR_HPP
