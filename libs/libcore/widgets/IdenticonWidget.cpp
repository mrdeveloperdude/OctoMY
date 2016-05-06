#include "IdenticonWidget.hpp"

#include "basic/UniquePlatformFingerprint.hpp"

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




void IdenticonWidget::regenerateIdenticon(){
	QDomDocument doc=identicon.domDocument();
	//TODO: This is a gaping hole if someone tried to set the SVG and did not expect us to delete it (not recommended way to use this class but still)
	if(0!=svg){
		delete svg;
		svg=0;
	}
	svg=new QSvgRenderer (doc.toByteArray());
	update();
}

void IdenticonWidget::setIdenticonData(quint64 data){
	identicon.setIdenticonData(data);
	regenerateIdenticon();
}


void IdenticonWidget::setSvgURL(QString url){
	identicon.setSvgURL(url);
	regenerateIdenticon();
}
