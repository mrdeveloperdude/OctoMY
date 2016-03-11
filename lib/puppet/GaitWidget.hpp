#ifndef GAITWIDGET_HPP
#define GAITWIDGET_HPP

#include <QWidget>
#include <QTimer>

#include "GaitController.hpp"

class GaitWidget : public QWidget{
		Q_OBJECT
	private:
		GaitController<qreal> gait;
		QTimer timer;
	public:
		explicit GaitWidget(QWidget *parent = 0);

	protected:
		void paintEvent(QPaintEvent *);
		void mouseReleaseEvent ( QMouseEvent * event ) ;
		void  keyPressEvent(QKeyEvent *event);

	public slots:
		void onUpdateTimer();

};

#endif // GAITWIDGET_HPP
