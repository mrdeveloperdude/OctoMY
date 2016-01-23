#ifndef MULTIVIEW_HPP
#define MULTIVIEW_HPP

#include <QWidget>

namespace Ui {
	class MultiView;
}

class MultiView : public QWidget
{
		Q_OBJECT

	public:
		explicit MultiView(QWidget *parent = 0);
		~MultiView();

	private:
		Ui::MultiView *ui;
};

#endif // MULTIVIEW_HPP
