#ifndef TRIGGERWIDGET_HPP
#define TRIGGERWIDGET_HPP

#include <QWidget>

class Trigger;

namespace Ui
{
class TriggerWidget;
}

class TriggerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::TriggerWidget *ui;
	Trigger *mTrigger;
public:
	explicit TriggerWidget(QWidget *parent = nullptr);
	~TriggerWidget();
public:

	void configure(Trigger *trigger);

private slots:
	void on_checkBoxEnabled_toggled(bool checked);
};

#endif
// TRIGGERWIDGET_HPP
