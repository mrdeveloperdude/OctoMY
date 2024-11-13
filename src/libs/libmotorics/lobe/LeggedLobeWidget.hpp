#ifndef LEGGEDLobeWIDGET_HPP
#define LEGGEDLobeWIDGET_HPP

#include "LobeWidget.hpp"
#include <QWidget>

namespace Ui
{
class LeggedLobeWidget;
}

class LeggedLobeWidget : public LobeWidget
{
	Q_OBJECT
private:
	Ui::LeggedLobeWidget *ui;

public:
	explicit LeggedLobeWidget(QWidget *parent = nullptr);
	~LeggedLobeWidget();

private slots:
	void on_pushButtonDelete_clicked();

};

#endif
// LEGGEDLobeWIDGET_HPP
