#ifndef PLANEDITOR_HPP
#define PLANEDITOR_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QKeyEvent>
#include <QTimer>
#include <QWidget>


class Node;
class OctomyParse;
class Plan;
class PlanHighlighter;

namespace Ui
{
class PlanEditor;
}


namespace plan{
namespace editor{

class PlanEditor : public QWidget
{
	Q_OBJECT
	
private:
	Ui::PlanEditor *ui;
	QSharedPointer<Node> mNode;
	QSharedPointer<Plan> mPlan;
	PlanHighlighter *mHighlighter{nullptr};
	QTimer mSaveTimer;
	QTimer mClockTimer;

public:
	explicit PlanEditor(QWidget *parent = nullptr);
	virtual ~PlanEditor() override;
	
public:
	void configure(QSharedPointer<Node> n);
	void loadPlan(const QString &id);
	
private:
	void setPlan(QSharedPointer<Plan> plan);

protected:
	void keyPressEvent(QKeyEvent *e) override;
	
public slots:
	void onTextChanged();
	void enableSaveButton();
	void tidyPlan();
	void parsePlan();
	void synchronizePlan();
	void clockUpdate();
	void loadDefaultPlan();
	void setPlanName(const QString &name);
};

}
}

#endif
// PLANEDITOR_HPP

