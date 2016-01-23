#ifndef SVGWIDGET_HPP
#define SVGWIDGET_HPP

#include <QWidget>
#include <QSvgRenderer>

class SvgWidget : public QWidget{
		Q_OBJECT
		Q_PROPERTY(QString svgURL READ svgURL WRITE setSvgURL DESIGNABLE true)
	protected:
		QSvgRenderer *svg;
		QString lastURL;
	public:
		explicit SvgWidget(QWidget *parent = 0);
		virtual ~SvgWidget();

		void setSvg(QSvgRenderer &svg);
		void setSvgURL(QString url);
		QString svgURL() const;

	protected:
		void paintEvent(QPaintEvent *);

};

#endif // SVGWIDGET_HPP
