#ifndef TRIGGERMANAGERWIDGET_HPP
#define TRIGGERMANAGERWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QList>

class TriggerExecutor;
class TriggerWidget;
class Trigger;

namespace Ui
{
class TriggerManagerWidget;
}

class TriggerManagerWidget : public QWidget
{
	Q_OBJECT

private:
	Ui::TriggerManagerWidget *ui;
	QList<QSharedPointer< QWidget> > mWidgets;
	QSharedPointer<TriggerExecutor> mTriggers;
	ConfigureHelper mConfigureHelper;

public:
	explicit TriggerManagerWidget(QWidget *parent = nullptr);
	~TriggerManagerWidget();

public:
	void configure(QSharedPointer<TriggerExecutor> set);

private:
	void addWidget(QSharedPointer<QWidget> w);
	QSharedPointer<TriggerWidget> addTriggerWidget(QSharedPointer<Trigger> trigger);
	void removeWidget(QSharedPointer<QWidget> w);

public slots:
	void resetExecution();
	void addTrigger();
	void stepExecution();
	void runExecution(bool run);
};

QSharedPointer<TriggerExecutor> mockTriggers();

#endif
// TRIGGERMANAGERWIDGET_HPP
