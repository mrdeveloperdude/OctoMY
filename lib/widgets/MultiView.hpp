#ifndef MULTIVIEW_HPP
#define MULTIVIEW_HPP

#include <QWidget>

class QAbstractItemModel;
class QAbstractButton;

namespace Ui {
	class MultiView;
}

class MultiView : public QWidget{
		Q_OBJECT
	private:
		QAbstractItemModel *data ;
		QString k;

	public:
		explicit MultiView(QWidget *parent = 0);
		~MultiView();

	public:
		void configure(QString val, QString key);
		void setModel(QAbstractItemModel *data);

	private:
		void setView(QString view);
	private slots:
		void onViewButtonClicked(QAbstractButton*);

	private:
		Ui::MultiView *ui;
};

#endif // MULTIVIEW_HPP
