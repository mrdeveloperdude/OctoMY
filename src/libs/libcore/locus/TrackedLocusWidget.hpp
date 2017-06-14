#ifndef TRACKEDLOCUSWIDGET_HPP
#define TRACKEDLOCUSWIDGET_HPP
#include "LocusWidget.hpp"

#include <QWidget>

namespace Ui
{
class TrackedLocusWidget;
}

class TrackedLocusWidget : public LocusWidget
{
	Q_OBJECT

public:
	explicit TrackedLocusWidget(QWidget *parent = 0);
	~TrackedLocusWidget();

private:
	Ui::TrackedLocusWidget *ui;
};

#endif // TRACKEDLOCUSWIDGET_HPP
