#ifndef HOVERINGMTLOBESWIDGET_HPP
#define HOVERINGMTLOBESWIDGET_HPP

#include "MtlobeWidget.hpp"

#include <QWidget>

namespace Ui
{
class HoveringMtlobeWidget;
}

class HoveringMtlobeWidget : public MtlobeWidget
{
	Q_OBJECT

public:
	explicit HoveringMtlobeWidget(QWidget *parent = 0);
	~HoveringMtlobeWidget();

private:
	Ui::HoveringMtlobeWidget *ui;
};

#endif // HOVERINGMTLOBESWIDGET_HPP
