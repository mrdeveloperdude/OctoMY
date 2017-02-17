#ifndef WHEELEDLOCUSWIDGET_HPP
#define WHEELEDLOCUSWIDGET_HPP

#include <QWidget>

namespace Ui
{
class WheeledLocusWidget;
}

class WheeledLocusWidget : public QWidget
{
	Q_OBJECT

public:
	explicit WheeledLocusWidget(QWidget *parent = 0);
	~WheeledLocusWidget();

	private slots:
		void on_pushButtonName_clicked();

	private:
	Ui::WheeledLocusWidget *ui;
};

#endif // WHEELEDLOCUSWIDGET_HPP
