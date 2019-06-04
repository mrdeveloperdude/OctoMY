#include "IdenticonWidget.hpp"

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


IdenticonWidget::IdenticonWidget(QWidget *parent)
	: SvgWidget(parent)
	, mConfigureHelperIdenticonWidget("IdenticonWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	// NOTE: We need configure here because of how decendents of QWidget are initialized
	configureIdenticon();
}

void IdenticonWidget::configureIdenticon()
{
	OC_METHODGATE();
	if(mConfigureHelperIdenticonWidget.configure()) {
		SvgWidget::configure(false);
		PortableID pid;
		setPortableID(pid);
	}
}


void IdenticonWidget::regenerateIdenticon()
{
	OC_METHODGATE();
	if(mConfigureHelperIdenticonWidget.isConfiguredAsExpected()) {
		//TODO: This is a gaping hole if someone tried to set the SVG and did not expect us to delete it (not recommended way to use this class but still)
		if(nullptr!=mSVG) {
			delete mSVG;
			mSVG=nullptr;
		}
		QDomDocument doc=mIdenticon.domDocument();
		mSVG=OC_NEW QSvgRenderer (doc.toByteArray());
		mLastURL="";
		mDirty=true;
		update();
	}
}


void IdenticonWidget::setPortableID(PortableID &id)
{
	OC_METHODGATE();
	if(mConfigureHelperIdenticonWidget.isConfiguredAsExpected()) {
		mIdenticon.setPortableID(id);
		setToolTip("ID="+id.id()+"\nNAME="+id.name()+"\nTYPE="+nodeTypeToString(id.type())+"\nBIRTH="+utility::string::formattedDateFromMS(id.birthDate())+"\n");
		regenerateIdenticon();
	}
}


Identicon IdenticonWidget::identicon()
{
	OC_METHODGATE();
	if(mConfigureHelperIdenticonWidget.isConfiguredAsExpected()) {
		return mIdenticon;
	}
	return Identicon();
}


void IdenticonWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelperIdenticonWidget.isConfiguredAsExpected()) {
		emit doubleClicked();
	}
}


QDomDocument IdenticonWidget::svgDOM()
{
	OC_METHODGATE();
	if(mConfigureHelperIdenticonWidget.isConfiguredAsExpected()) {
		return mIdenticon.domDocument();
	}
	return QDomDocument();
}


QByteArray IdenticonWidget::svgXML()
{
	OC_METHODGATE();
	if(mConfigureHelperIdenticonWidget.isConfiguredAsExpected()) {
		return mIdenticon.domDocument().toByteArray();
	}
	return QByteArray();
}
