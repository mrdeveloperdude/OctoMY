#ifndef TRIGGERWIDGET_HPP
#define TRIGGERWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>

class Trigger;
class CompositeCondition;

namespace Ui
{
class TriggerWidget;
}

class TriggerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::TriggerWidget *ui;
	QMenu *mConditionMenu{nullptr};
	QMenu *mActionMenu{nullptr};
	QSharedPointer<Trigger> mTrigger;
	QSharedPointer<CompositeCondition> mCurrentCondition;
	bool mEditMode{false};
	ConfigureHelper mConfigureHelper;

public:
	explicit TriggerWidget(QWidget *parent = nullptr);
	~TriggerWidget();
	
private:
	QAction* addConditionMenuItem(QString title, QString icon, QString toolTip);
	QAction* addActionMenuItem(QString title, QString icon, QString toolTip);
	void prepareMenus();
	void prepareConditions();
	void updateConditions();
	void updateActions();
	
	void configureConditionsList();
	void configureActionsList();


public:
	void configure(QSharedPointer<Trigger> trigger);

private slots:
	void remove();
	void toggleEnabled(bool enabled);
	void toggleEdit(bool edit);
	void addCondition(const QString &name);
	void addAction(const QString &name);
	
	
protected:
	void enterEvent(QEnterEvent *event) override;
	void leaveEvent(QEvent *event) override;

};

#endif
// TRIGGERWIDGET_HPP
