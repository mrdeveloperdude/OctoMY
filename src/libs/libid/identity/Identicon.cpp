#include "Identicon.hpp"
#include "basic/UniquePlatformFingerprint.hpp"
#include "rng/RNG.hpp"

#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>
#include <QDebug>
#include <QFile>

#include <qmath.h>

Identicon::Identicon(PortableID &id)
	: mDirty(true)
	, mID(id)
	, mColors(mID.id())
{
	setPortableID(id);
}

Identicon::Identicon()
	: mDirty(true)
	, mColors(mID.id())
{
}

Identicon::~Identicon()
{
}



static void setAttrRecur(QDomElement &elem, QString tag, QString id, QString attr, QString val)
{
	if (0==elem.tagName().compare(tag) && elem.hasAttribute("id") && 0==elem.attribute("id").compare(id)) {
		elem.setAttribute(attr, val);
	}
	for (int i = 0, l=elem.childNodes().count(); i < l; ++i) {
		if (!elem.childNodes().at(i).isElement()) {
			continue;
		}
		QDomElement o=elem.childNodes().at(i).toElement();
		setAttrRecur(o, tag, id, attr, val);
	}
}


static QString colorToSvgString(const QColor &c)
{
	return "rgb("+QString::number(c.red())+", "+QString::number(c.green())+", "+QString::number(c.blue())+")";
}


float Identicon::frand()
{
	return mPersonality.rng().generateReal1();
}


QDomDocument Identicon::domDocument()
{
	regenerateIdenticon();
	return mDoc;
}

static QSizeF calcSize(QSizeF ds,qint32 w,qint32 h,qreal zoom)
{
	if(w<1 && h<1) {
		return ds;
	}
	const qreal da=ds.width()/ds.height();
	//	const qreal ca=((qreal)(w<1?1:w)/(qreal)(h<1?1:h));
	QSizeF low(w,(qreal)h/da);
	QSizeF high((qreal)w*da,h);
	//qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
	zoom=(zoom<0)?0:(zoom>1)?1:zoom;
	qreal izoom=1.0-zoom;
	return low*izoom+high*zoom;
}

QPixmap Identicon::pixmap(qint32 w,qint32 h,qreal zoom)
{
	regenerateIdenticon();
	QSvgRenderer svg( mDoc.toByteArray() );
	QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
	//qDebug()<<"Generating pixmap from identicon with size: "<<ds;
	QPixmap px(ds.toSize());
	px.fill(QColor(0,0,0,0));
	QPainter painter( &px );
	svg.render( &painter );
	//px.save("/tmp/px.png");
	return px;
}


QImage Identicon::image(qint32 w,qint32 h,qreal zoom)
{
	regenerateIdenticon();
	QSvgRenderer svg( mDoc.toByteArray() );
	QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
	//qDebug()<<"Generating image from identicon with size: "<<ds;
	QImage px(ds.toSize(), QImage::Format_RGBA8888);
	px.fill(QColor(0,0,0,0));
	QPainter painter( &px );
	svg.render( &painter );
	return px;
}


void Identicon::erectLimb(QDomElement &o, QString limbStyle, bool top, bool left, bool mid,float p1,float p2,float p3,float p4,float p5,float p6)
{
	const QString name=QString("agent_limb_")+(left?"left_":"right_")+(top?"top_":"bot_")+(mid?"mid":(top?"top":"bot"));
	QString m= "m ";
	const float dirAngle=((top?p1:p3)-0.75)*0.5*M_PI_2;
	const float spreadAngle=(top?p2:p4)*0.5*M_PI_2;
	const float startAngle=dirAngle+spreadAngle*(mid?1.0:-1.0);
	const float centerx=-882.65796;
	const float centery=top?601.55292:622.47913;
	const float startofx=16.92954;

	const float startx=centerx+cos(startAngle)*startofx*(left?-1:1);
	const float starty=centery+sin(startAngle)*startofx*(top?1:-1);

	const int limbSteps=2;
	const float limbSize=7.5;
	const float limbStepSize=limbSize/limbSteps;

	//First step
	m+=QString::number(startx)+", "+QString::number(starty)+" ";
	float angle=startAngle;
	for(int step=0; step<limbSteps; ++step) {
		angle+=(((top?p5:p6)-0.5)*2 *M_PI_2)/limbSteps;
		const float stepX=cos(angle)*limbStepSize*(left?-1:1);
		const float stepY=sin(angle)*limbStepSize*(top?1:-1);
		m+=QString::number(stepX)+", "+QString::number(stepY)+" ";
	}
	//qDebug()<<"LIMB: "<<name<<m << " dirAngle="<<dirAngle<<" spreadAngle=" << spreadAngle<<" startAngle=" << startAngle;
	setAttrRecur(o, "path", name, "d", m);
	setAttrRecur(o, "path", name, "style", limbStyle);

}

void Identicon::regenerateIdenticon()
{
	if(mDirty) {
		QString url;
		switch(mID.type()) {
		default:
		case(TYPE_AGENT):
			url=":/icons/agent.svg";
			break;
		case(TYPE_REMOTE):
			url=":/icons/remote.svg";
			break;
		case(TYPE_HUB):
			url=":/icons/hub.svg";
			break;
		}

		if(!url.isEmpty()) {
			if(!url.isNull()) {
				QFile file(url);
				if(file.exists()) {
					bool ret=file.open(QIODevice::ReadOnly);
					if(ret) {
						QByteArray baData = file.readAll();
						mDoc.setContent(baData);
						// When ID is not set, default is original colors of the file (should be official OctoMY colors)
						if(""!=mID.id()) {
							mPersonality.reset();
							//qreal p1=frand();						qreal p2=frand();						qreal p3=frand();
							qreal p4=frand();
							qreal p5=frand();
							qreal p6=frand();
							qreal p7=frand();
							qreal p8=frand();
							qreal p9=frand();
							//qreal p10=frand();

							//qDebug()<<"Identicon params: "<<mID.id()<<p1<<p2<<p3<<p4<<p5<<p6<<p7;

							const QString bodyColorHighStr=colorToSvgString(mColors.bodyColorHigh());
							const QString bodyColorLowStr=colorToSvgString(mColors.bodyColorLow());
							const QString backgroundColorHighStr=colorToSvgString(mColors.backgroundColorHigh());
							const QString backgroundColorLowStr=colorToSvgString(mColors.backgroundColorLow());
							const QString limbColorStr=colorToSvgString(mColors.limbColor());

							QDomElement o=mDoc.documentElement();

							DiscoveryType type=mID.type();

							if(TYPE_AGENT==type) {
								setAttrRecur(o, "stop", "agent_body_color_stop1", "style", "stop-color:"+bodyColorHighStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "agent_body_color_stop2", "style", "stop-color:"+bodyColorLowStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "agent_background_color_stop1", "style", "stop-color:"+backgroundColorHighStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "agent_background_color_stop2", "style", "stop-color:"+backgroundColorLowStr+";stop-opacity:1");
								setAttrRecur(o, "path", "agent_face", "style", "fill:none;stroke:"+limbColorStr+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");

								const QString limbStyle="fill:none;stroke:"+limbColorStr+";stroke-width:6.12427092;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1";

								erectLimb(o,limbStyle, false, false,false,p4,p5,p6,p7,p8,p9);
								erectLimb(o,limbStyle, true,false,false,p4,p5,p6,p7,p8,p9);
								erectLimb(o,limbStyle, false,true,false,p4,p5,p6,p7,p8,p9);
								erectLimb(o,limbStyle, true,true,false,p4,p5,p6,p7,p8,p9);
								erectLimb(o,limbStyle, false,false,true,p4,p5,p6,p7,p8,p9);
								erectLimb(o,limbStyle, true,false,true,p4,p5,p6,p7,p8,p9);
								erectLimb(o,limbStyle, false,true,true,p4,p5,p6,p7,p8,p9);
								erectLimb(o,limbStyle, true,true,true,p4,p5,p6,p7,p8,p9);
							} else if(TYPE_REMOTE==type) {
								setAttrRecur(o, "stop", "remote_body_color_stop1", "style", "stop-color:"+bodyColorHighStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "remote_body_color_stop2", "style", "stop-color:"+bodyColorLowStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "remote_background_color_stop1", "style", "stop-color:"+backgroundColorHighStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "remote_background_color_stop2", "style", "stop-color:"+backgroundColorLowStr+";stop-opacity:1");
								setAttrRecur(o, "path", "remote_face", "style", "fill:none;stroke:"+limbColorStr+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");

								setAttrRecur(o, "circle", "remote_outer_ring", "style", "fill:none;stroke:"+limbColorStr+";stroke-width:" + QString::number(0.25+p4*3)+ ";stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
								setAttrRecur(o, "circle", "remote_middle_ring", "style", "fill:none;stroke:"+limbColorStr+";stroke-width:" + QString::number(0.20+p5*3.5)+ ";stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
								setAttrRecur(o, "circle", "remote_inner_ring", "style", "fill:none;stroke:"+limbColorStr+";stroke-width:" + QString::number(0.25+p6*3)+ ";stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");

								setAttrRecur(o, "circle", "remote_middle_ring", "r", QString::number(11+ p7*6.0));

								setAttrRecur(o, "circle", "remote_ball", "style", "stroke:none; fill:"+limbColorStr+";");
							} else if(TYPE_HUB==type) {
								setAttrRecur(o, "stop", "hub_body_color_stop1", "style", "stop-color:"+bodyColorHighStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "hub_body_color_stop2", "style", "stop-color:"+bodyColorLowStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "hub_background_color_stop1", "style", "stop-color:"+backgroundColorHighStr+";stop-opacity:1");
								setAttrRecur(o, "stop", "hub_background_color_stop2", "style", "stop-color:"+backgroundColorLowStr+";stop-opacity:1");
								setAttrRecur(o, "path", "hub_face", "style", "fill:none;stroke:"+limbColorStr+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
								setAttrRecur(o, "path", "hub_brain", "style", "fill:none;stroke:"+limbColorStr+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
								setAttrRecur(o, "path", "hub_brain", "transform", "rotate("+QString::number(p4*360.0)+", -860.79895, 612.97186)");
							}
						}
						mDirty=false;
					} else {
						qWarning ()<<"ERROR: Could not open file by URL: "<<url;
					}
				} else {
					qWarning ()<<"ERROR: file not found while regenerating identicon from URL: "<<url;
				}
			} else {
				qWarning()<<"ERROR: URL is null";
			}
		} else {
			qWarning()<<"ERROR: URL is empty";
		}
	}
}

/*
void Identicon::setSvgURL(QString url){
	this->url=url;
	dirty=true;
}*/

PortableID &Identicon::id()
{
	return mID;
}

void Identicon::setPortableID(PortableID pid)
{
	mID=pid;
	QString id=mID.id();
	mPersonality.setID(id, Personality::IDENTICON);
	mColors.setID(id);
	mDirty=true;
	regenerateIdenticon();
}
