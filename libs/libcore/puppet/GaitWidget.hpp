#ifndef GAITWIDGET_HPP
#define GAITWIDGET_HPP

#include <QWidget>
#include <QTimer>

#include "GaitController.hpp"

class GaitWidget : public QWidget{
		Q_OBJECT
	private:
		GaitController<qreal> *m_gait;
	public:
		explicit GaitWidget(QWidget *parent = 0);

	public:

		void setGait(GaitController <qreal> &gait);


	public slots:

		void onGaitUpdated();


	protected:
		void paintEvent(QPaintEvent *);
		void mouseReleaseEvent ( QMouseEvent * event ) ;
		void mouseMoveEvent(QMouseEvent *event);
		void keyPressEvent(QKeyEvent *event);

};

#endif // GAITWIDGET_HPP
