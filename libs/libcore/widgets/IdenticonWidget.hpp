#ifndef IDENTICONWIDGET_HPP
#define IDENTICONWIDGET_HPP

#include "widgets/SvgWidget.hpp"
#include "Identicon.hpp"


#include <QDomDocument>


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
		void setIdenticonData(QByteArray);

	signals:
		void doubleClicked();

	protected:

		void mouseDoubleClickEvent(QMouseEvent *);




};

#endif // IDENTICONWIDGET_HPP



