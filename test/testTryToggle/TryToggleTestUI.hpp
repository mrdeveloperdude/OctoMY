#ifndef TRYTOGGLETESTUI_HPP
#define TRYTOGGLETESTUI_HPP

#include "components/TryToggleState.hpp"

#include <QWidget>


namespace Ui
{
class TryToggleTestUI;
}


class TryToggleTestUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TryToggleTestUI *ui;

public:
	explicit TryToggleTestUI(QWidget *parent = nullptr);
	~TryToggleTestUI();

private slots:
	void on_pushButtonOff_clicked();
	void on_pushButtonGoingOn_clicked();
	void on_pushButtonOn_clicked();
	void on_pushButtonGoingOff_clicked();

private slots:
	void onStateChanged(TryToggleState from,TryToggleState to);
};


#endif
// TRYTOGGLETESTUI_HPP
