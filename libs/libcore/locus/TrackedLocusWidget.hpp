#ifndef TRACKEDLOCUSWIDGET_HPP
#define TRACKEDLOCUSWIDGET_HPP

#include <QWidget>

namespace Ui
{
class TrackedLocusWidget;
}

class TrackedLocusWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TrackedLocusWidget(QWidget *parent = 0);
	~TrackedLocusWidget();

private:
	Ui::TrackedLocusWidget *ui;
};

#endif // TRACKEDLOCUSWIDGET_HPP
