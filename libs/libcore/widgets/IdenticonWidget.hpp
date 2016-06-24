#ifndef IDENTICONWIDGET_HPP
#define IDENTICONWIDGET_HPP

#include "widgets/SvgWidget.hpp"
#include "Identicon.hpp"


#include <QDomDocument>

class PortableI;

class IdenticonWidget : public SvgWidget{
		Q_OBJECT

	private:
		Identicon identicon;

	public:
		explicit IdenticonWidget(QWidget *parent = 0);

	protected:

		void regenerateIdenticon();

	public:
		void setPortableID(PortableID &id);

	signals:
		void doubleClicked();

	protected:

		void mouseDoubleClickEvent(QMouseEvent *);




};

#endif // IDENTICONWIDGET_HPP



