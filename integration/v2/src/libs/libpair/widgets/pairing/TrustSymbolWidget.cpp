#include "TrustSymbolWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "security/PortableID.hpp"
#include "node/NodeRole.hpp"
#include "node/NodeType.hpp"
#include "utility/string/String.hpp"

#include <QPainter>
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QSvgRenderer>


TrustSymbolWidget::TrustSymbolWidget(QWidget *parent)
	: SvgWidget(parent)
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(icons);
}


void TrustSymbolWidget::regenerateIdenticon()
{
	OC_METHODGATE();
	//TODO: This is a gaping hole if someone tried to set the SVG and did not expect us to delete it (not recommended way to use this class but still)
	if(nullptr!=mSVG) {
		delete mSVG;
		mSVG=nullptr;
	}
//	QDomDocument doc=mIdenticon.domDocument();
//	mSVG=OC_NEW QSvgRenderer (doc.toByteArray());
	mLastURL="";
	mDirty=true;
	update();
}


void TrustSymbolWidget::setTrustLevel(TrustLevel level)
{
	OC_METHODGATE();
	QString file;
	switch(level) {
	case(TRUST): {
		file=":/icons/trust.svg";
	}
	break;
	case(BLOCK): {
		file=":/icons/block.svg";
	}
	break;
	// default:
	case(IGNORE): {
		file=":/icons/ignore.svg";
	}
	break;

	}

	setSvgURL(file);

	//mIdenticon.setPortableID(id);
	//setToolTip("ID="+id.id()+"\nNAME="+id.name()+"\nTYPE="+nodeTypeToString(id.type())+"\nBIRTH="+utility::string::formattedDateFromMS(id.birthDate())+"\n");
	regenerateIdenticon();
}


void TrustSymbolWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	OC_METHODGATE();
	emit doubleClicked();
}

/*
QDomDocument TrustSymbolWidget::svgDOM()
{
	OC_METHODGATE();
	return QDomDocument();
}


QByteArray TrustSymbolWidget::svgXML()
{
	OC_METHODGATE();
	return mIdenticon.domDocument().toByteArray();
}
*/
