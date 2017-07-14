#ifndef TRACKEDMTLOBEWIDGET_HPP
#define TRACKEDMTLOBEWIDGET_HPP
#include "MtlobeWidget.hpp"

#include <QWidget>

namespace Ui
{
class TrackedMtlobeWidget;
}

class TrackedMtlobeWidget : public MtlobeWidget
{
	Q_OBJECT

public:
	explicit TrackedMtlobeWidget(QWidget *parent = 0);
	~TrackedMtlobeWidget();

private:
	Ui::TrackedMtlobeWidget *ui;
};

#endif // TRACKEDMTLOBEWIDGET_HPP
