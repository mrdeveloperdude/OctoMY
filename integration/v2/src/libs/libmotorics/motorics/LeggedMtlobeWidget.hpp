#ifndef LEGGEDMTLOBEWIDGET_HPP
#define LEGGEDMTLOBEWIDGET_HPP

#include "MtlobeWidget.hpp"
#include <QWidget>

namespace Ui
{
class LeggedMtlobeWidget;
}

class LeggedMtlobeWidget : public MtlobeWidget
{
	Q_OBJECT
private:
	Ui::LeggedMtlobeWidget *ui;

public:
	explicit LeggedMtlobeWidget(QWidget *parent = nullptr);
	~LeggedMtlobeWidget();

private slots:
	void on_pushButtonDelete_clicked();

};

#endif
// LEGGEDMTLOBEWIDGET_HPP
