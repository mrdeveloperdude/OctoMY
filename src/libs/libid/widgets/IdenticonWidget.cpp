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
}

void IdenticonWidget::regenerateIdenticon()
{
	//TODO: This is a gaping hole if someone tried to set the SVG and did not expect us to delete it (not recommended way to use this class but still)
	if(nullptr!=mSVG) {
		delete mSVG;
		mSVG=nullptr;
	}
	QDomDocument doc=identicon.domDocument();
	mSVG=new QSvgRenderer (doc.toByteArray());
	mLastURL="";
	mDirty=true;
	update();
}

void IdenticonWidget::setPortableID(PortableID &id)
{
	identicon.setPortableID(id);
	regenerateIdenticon();
}


void IdenticonWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}
