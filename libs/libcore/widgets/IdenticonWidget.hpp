#ifndef IDENTICONWIDGET_HPP
#define IDENTICONWIDGET_HPP

#include "widgets/SvgWidget.hpp"
#include <QWidget>
#include <QSvgRenderer>


#include <QDomDocument>

class Identicon{
	private:
		quint64 data;
		QString url;
		bool dirty;
		QByteArray ba;
		QDomDocument doc;
	public:
		explicit Identicon(QString url="", quint64 data=0);
		virtual  ~Identicon();

	public:
		void setSvgURL(QString);
		void setIdenticonData(quint64);

		QDomDocument domDocument();

		QPixmap pixmap();

	private:

		void regenerateIdenticon();
};

class IdenticonWidget : public SvgWidget{
		Q_OBJECT

		Q_PROPERTY(QString svgURL READ svgURL WRITE setSvgURL DESIGNABLE true)

	private:
		Identicon identicon;

	public:
		explicit IdenticonWidget(QWidget *parent = 0);

		void setSvgURL(QString url);

	protected:

		void regenerateIdenticon();

	public:
		void setIdenticonData(quint64);

};

#endif // IDENTICONWIDGET_HPP



