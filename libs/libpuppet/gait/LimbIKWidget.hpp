#ifndef LIMBIKWIDGET_HPP
#define LIMBIKWIDGET_HPP

#include <QWidget>

struct IKLimb;

class LimbIKWidget : public QWidget
{
		Q_OBJECT

	private:

		IKLimb *limb;
	public:
		explicit LimbIKWidget(QWidget *parent = 0);

	public:

		void getLimbVars(qreal &cox, qreal &fem, qreal &tib );

	protected:
		void paintEvent(QPaintEvent *);
		void mousePressEvent ( QMouseEvent * event ) ;
		void mouseReleaseEvent ( QMouseEvent * event ) ;
		void mouseMoveEvent(QMouseEvent *event);
		void keyPressEvent(QKeyEvent *event);
	signals:

		void IKUpadted();
};

#endif // LIMBIKWIDGET_HPP
