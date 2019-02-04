#ifndef WHEELEDMTLOBEWIDGET_HPP
#define WHEELEDMTLOBEWIDGET_HPP

#include "MtlobeWidget.hpp"

#include <QWidget>


class WheeledMtlobe;
namespace Ui
{
class WheeledMtlobeWidget;
}

class WheeledMtlobeWidget : public MtlobeWidget
{
	Q_OBJECT

private:
	Ui::WheeledMtlobeWidget *ui;
	WheeledMtlobe *mWheeledMtlobe;

public:
	explicit WheeledMtlobeWidget(QWidget *parent = nullptr);
	~WheeledMtlobeWidget();

public:
	void configure(WheeledMtlobe *wheeledMtlobe);


private slots:
	void on_pushButtonName_clicked();
	void on_pushButtonDelete_clicked();

};

#endif
// WHEELEDMTLOBEWIDGET_HPP
