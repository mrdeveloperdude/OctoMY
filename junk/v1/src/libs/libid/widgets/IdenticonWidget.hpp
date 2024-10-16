#ifndef IDENTICONWIDGET_HPP
#define IDENTICONWIDGET_HPP

#include "widgets/SvgWidget.hpp"
#include "identity/Identicon.hpp"


#include <QDomDocument>

class PortableI;

class IdenticonWidget : public SvgWidget
{
	Q_OBJECT

private:
	Identicon mIdenticon;

public:
	explicit IdenticonWidget(QWidget *parent = nullptr);

protected:
	void regenerateIdenticon();

public:
	void setPortableID(PortableID &id);

	QDomDocument svgDOM();

	QByteArray svgXML();

	Identicon identicon();
signals:
	void doubleClicked();

protected:
	void mouseDoubleClickEvent(QMouseEvent *);

};

#endif // IDENTICONWIDGET_HPP

