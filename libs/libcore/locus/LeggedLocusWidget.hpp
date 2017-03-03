#ifndef LEGGEDLOCUSWIDGET_HPP
#define LEGGEDLOCUSWIDGET_HPP

#include "LocusWidget.hpp"
#include <QWidget>

namespace Ui
{
class LeggedLocusWidget;
}

class LeggedLocusWidget : public LocusWidget
{
	Q_OBJECT
private:
	Ui::LeggedLocusWidget *ui;

public:
	explicit LeggedLocusWidget(QWidget *parent = 0);
	~LeggedLocusWidget();

private slots:
	void on_pushButtonDelete_clicked();

};

#endif // LEGGEDLOCUSWIDGET_HPP
