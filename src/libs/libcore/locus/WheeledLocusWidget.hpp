#ifndef WHEELEDLOCUSWIDGET_HPP
#define WHEELEDLOCUSWIDGET_HPP

#include "LocusWidget.hpp"

#include <QWidget>


class WheeledLocus;
namespace Ui
{
class WheeledLocusWidget;
}

class WheeledLocusWidget : public LocusWidget
{
	Q_OBJECT
private:
	Ui::WheeledLocusWidget *ui;
	WheeledLocus *mWheeledLocus;


public:
	explicit WheeledLocusWidget(QWidget *parent = 0);
	~WheeledLocusWidget();

public:

	void configure(WheeledLocus *wheeledLocus);


private slots:
	void on_pushButtonName_clicked();

	void on_pushButtonDelete_clicked();

};

#endif // WHEELEDLOCUSWIDGET_HPP
