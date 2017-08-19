#include "IdenticonWidget.hpp"

#include "security/PortableID.hpp"

#include <QPainter>
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QSvgRenderer>

///////////////////////////////////////////////////////////////////////////

IdenticonWidget::IdenticonWidget(QWidget *parent)
	: SvgWidget(parent)
{
	PortableID pid;
	setPortableID(pid);
}

void IdenticonWidget::regenerateIdenticon()
{
	//TODO: This is a gaping hole if someone tried to set the SVG and did not expect us to delete it (not recommended way to use this class but still)
	if(nullptr!=mSVG) {
		delete mSVG;
		mSVG=nullptr;
	}
	QDomDocument doc=mIdenticon.domDocument();
	mSVG=new QSvgRenderer (doc.toByteArray());
	mLastURL="";
	mDirty=true;
	update();
}

void IdenticonWidget::setPortableID(PortableID &id)
{
	mIdenticon.setPortableID(id);
	regenerateIdenticon();
}

Identicon IdenticonWidget::identicon()
{
	return mIdenticon;
}

void IdenticonWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}


QDomDocument IdenticonWidget::svgDOM(){
	return mIdenticon.domDocument();
}

QByteArray IdenticonWidget::svgXML(){
	return mIdenticon.domDocument().toByteArray();
}
