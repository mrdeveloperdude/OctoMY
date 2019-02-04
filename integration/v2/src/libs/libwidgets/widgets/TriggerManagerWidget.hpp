#ifndef TRIGGERMANAGERWIDGET_HPP
#define TRIGGERMANAGERWIDGET_HPP

#include <QWidget>
#include <QList>

class TriggerSet;

namespace Ui
{
class TriggerManagerWidget;
}

class TriggerManagerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::TriggerManagerWidget *ui;
	QList<QWidget *> mWidgets;
	TriggerSet *mTriggers;

public:
	explicit TriggerManagerWidget(QWidget *parent = nullptr);
	~TriggerManagerWidget();

public:

	void configure(TriggerSet &set);

private:
	void addWidget(QWidget &w);
	void removeWidget(QWidget &w);

};


#endif
// TRIGGERMANAGERWIDGET_HPP
