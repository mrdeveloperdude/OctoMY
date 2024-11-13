#ifndef WHEELEDLobeWIDGET_HPP
#define WHEELEDLobeWIDGET_HPP

#include "LobeWidget.hpp"

#include <QWidget>


class WheeledLobe;
namespace Ui
{
class WheeledLobeWidget;
}

class WheeledLobeWidget : public LobeWidget
{
	Q_OBJECT

private:
	Ui::WheeledLobeWidget *ui;
	WheeledLobe *mWheeledLobe;

public:
	explicit WheeledLobeWidget(QWidget *parent = nullptr);
	~WheeledLobeWidget();

public:
	void configure(WheeledLobe *wheeledLobe);


private slots:
	void on_pushButtonName_clicked();
	void on_pushButtonDelete_clicked();

};

#endif
// WHEELEDLobeWIDGET_HPP
