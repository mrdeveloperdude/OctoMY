#ifndef LOBETYPESELECTOR_HPP
#define LOBETYPESELECTOR_HPP

#include <QWidget>
#include "components/navigation/Activity.hpp"

class Agent;
class LobeTypeModel;

namespace Ui {
class LobeTypeSelector;
}

class LobeTypeSelector : public Activity
{
	Q_OBJECT
private:
	Ui::LobeTypeSelector *ui;
	QSharedPointer<Agent> mAgent;
	LobeTypeModel *mLobeTypeModel{nullptr};

	
public:
	explicit LobeTypeSelector(QWidget *parent = nullptr);
	~LobeTypeSelector();
	
private:
	void updateList();
	
public:
	void configure(QSharedPointer<Agent> agent);
	void reset();
	
private slots:
	void selectLobe();
};

#endif // LOBETYPESELECTOR_HPP
