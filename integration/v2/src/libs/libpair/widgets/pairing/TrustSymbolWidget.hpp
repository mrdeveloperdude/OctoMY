#ifndef TRUSTSYMBOLWIDGET_HPP
#define TRUSTSYMBOLWIDGET_HPP

#include "widgets/SvgWidget.hpp"
#include "identity/Identicon.hpp"

#include "TrustLevel.hpp"

#include <QDomDocument>

class TrustSymbolWidget : public SvgWidget
{
	Q_OBJECT
private:


public:
	explicit TrustSymbolWidget(QWidget *parent = nullptr);

protected:
	void regenerateIdenticon();

public:
	void setTrustLevel(TrustLevel level);
	//QDomDocument svgDOM();
	//QByteArray svgXML();

protected:
	void mouseDoubleClickEvent(QMouseEvent *);

signals:
	void doubleClicked();

};


#endif
// TRUSTSYMBOLWIDGET_HPP
