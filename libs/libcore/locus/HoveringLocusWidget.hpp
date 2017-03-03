#ifndef HOVERINGLOCUSWIDGET_HPP
#define HOVERINGLOCUSWIDGET_HPP

#include <QWidget>

namespace Ui {
	class HoveringLocusWidget;
	}

class HoveringLocusWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit HoveringLocusWidget(QWidget *parent = 0);
		~HoveringLocusWidget();

	private:
		Ui::HoveringLocusWidget *ui;
};

#endif // HOVERINGLOCUSWIDGET_HPP
