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
private:
	Ui::TrackedMtlobeWidget *ui;

public:
	explicit TrackedMtlobeWidget(QWidget *parent = nullptr);
	~TrackedMtlobeWidget();
};

#endif
// TRACKEDMTLOBEWIDGET_HPP


