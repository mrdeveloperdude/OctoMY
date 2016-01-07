#ifndef SVGWIDGET_HPP
#define SVGWIDGET_HPP

#include <QWidget>
#include <QSvgRenderer>

class SvgWidget : public QWidget{
		Q_OBJECT
	private:
		QSvgRenderer r;
	public:
		explicit SvgWidget(QWidget *parent = 0);

	protected:
		void paintEvent(QPaintEvent *);

};

#endif // SVGWIDGET_HPP
