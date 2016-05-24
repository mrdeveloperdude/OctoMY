#include "Identicon.hpp"
#include "basic/UniquePlatformFingerprint.hpp"

#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>
#include <QDebug>
#include <QFile>





static void setAttrRecur(QDomElement &elem, QString tag, QString id, QString attr, QString val){
	if (0==elem.tagName().compare(tag) && elem.hasAttribute("id") && 0==elem.attribute("id").compare(id)){
		elem.setAttribute(attr, val);
	}
	for (int i = 0, l=elem.childNodes().count(); i < l; ++i){
		if (!elem.childNodes().at(i).isElement()){
			continue;
		}
		QDomElement o=elem.childNodes().at(i).toElement();
		setAttrRecur(o, tag, id, attr, val);
	}
}

/*
static float frand(){
	return (rand()/(float)RAND_MAX);
}

static QString hsla(qreal h, qreal s, qreal l, qreal a){
	return "hsla("+QString::number(h*360.0f)+", "+QString::number((int)(s*100))+"%, "+QString::number((int)(l*100))+"%, "+QString::number(a)+")";
}
*/

static QString hsl(qreal h, qreal s, qreal l){
	QColor c=QColor::fromHslF(h,s,l);
	return "rgb("+QString::number(c.red())+", "+QString::number(c.green())+", "+QString::number(c.blue())+")";
}



static qreal realBits(quint64 d,quint64 bits){
	//qDebug()<<"---realBits for data "<<d <<" with "<<bits <<" bits";
	const qreal d2=(d & bits);
	//qDebug()<<"framed: "<<d2;
	const qreal bitsf=(qreal)bits;
	qreal res=d2/bitsf;
	//qDebug()<<"res: "<<res;
	//qDebug()<<"\n";
	return res;
}



Identicon::Identicon(QString url, quint64 data)
	: data(1)//NOT 0 to trigger generation of default fingerprint below
	, url(url)
	, dirty(true)
{
	setIdenticonData(data);
}

Identicon::~Identicon(){

}

QDomDocument Identicon::domDocument(){
	regenerateIdenticon();
	return doc;
}

static QSizeF calcSize(QSizeF ds,qint32 w,qint32 h,qreal zoom){
	if(w<1 && h<1){
		return ds;
	}
	const qreal da=ds.width()/ds.height();
//	const qreal ca=((qreal)(w<1?1:w)/(qreal)(h<1?1:h));
	QSizeF low(w,(qreal)h/da);
	QSizeF high((qreal)w*da,h);
	qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
	zoom=(zoom<0)?0:(zoom>1)?1:zoom;
	qreal izoom=1.0-zoom;
	return low*izoom+high*zoom;
}

QPixmap Identicon::pixmap(qint32 w,qint32 h,qreal zoom){
	regenerateIdenticon();
	QSvgRenderer svg( doc.toByteArray() );
	QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
	qDebug()<<"Generating pixmap from identicon with size: "<<ds;
	QPixmap px(ds.toSize());
	px.fill(QColor(0,0,0,0));
	QPainter painter( &px );
	svg.render( &painter );
	//px.save("/tmp/px.png");
	return px;
}


QImage Identicon::image(qint32 w,qint32 h,qreal zoom){
	regenerateIdenticon();
	QSvgRenderer svg( doc.toByteArray() );
	QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
	qDebug()<<"Generating image from identicon with size: "<<ds;
	QImage px(ds.toSize(), QImage::Format_RGBA8888);
	px.fill(QColor(0,0,0,0));
	QPainter painter( &px );
	svg.render( &painter );
	return px;
}


void Identicon::regenerateIdenticon(){
	if(dirty){
		if(url.isEmpty() || url.isNull()){
			qWarning()<<"ERROR: URL is invalid, aborting render of identicon SVG.";
		}
		else{
			QFile file(url);
			if(!file.exists()){
				qWarning ()<<"ERROR: file not found while regenerating identicon from URL: "<<url;
			}
			else{
				file.open(QIODevice::ReadOnly);
				QByteArray baData = file.readAll();
				doc.setContent(baData);
				auto o=doc.documentElement();
				quint64 d=data;
				qreal p1=0.0;
				qreal p2=0.0;
				qreal p3=0.0;
				qreal p4=0.0;
				qreal p5=0.0;
				qreal p6=0.0;
				//	qreal p7=0.0;
				//	qreal p8=0.0;
				//	qreal p9=0.0;
				bool debug=false;
				if(debug){
					qreal dd=(data/20.0);
					p1=dd;
					p2=dd;
					p3=dd;
					p4=dd;
					p5=dd;
					p6=dd;
					//		p7=dd;
					//		p8=dd;
					//		p9=dd;
				}
				else{
					//	qDebug()<<"DATA: "<<d;
					p1=realBits(d, 0xff);d=(d>>8);
					p2=realBits(d, 0xff);d=(d>>8);
					p3=realBits(d, 0xff);d=(d>>8);
					p4=realBits(d, 0xff);d=(d>>8);
					p5=realBits(d, 0xff);d=(d>>8);
					p6=realBits(d, 0xff);d=(d>>8);
					//		p7=realBits(d, 0xff);d=(d>>8);
					//		p8=realBits(d, 0xff);d=(d>>8);
					//		p9=realBits(d, 0xff);d=(d>>8);
				}
				//qDebug()<<"Identicon params: "<<p1<<p2<<p3<<p4<<p5<<p6<<p7<<p8<<p9;
				const QString c1=hsl(p1,0.75f,0.55f);
				const QString c2=hsl(p2,0.75f,0.55f);
				const QString c3=hsl(p3,0.75f,0.25f);
				setAttrRecur(o, "circle", "circleTop", "style", "stroke:" + c1+ ";fill:" + c3+ ";opacity:1;fill-opacity:1;fill-rule:evenodd;stroke-width:3.86077285;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1");
				setAttrRecur(o, "circle", "circleBottom", "style", "stroke:" + c1+ ";fill:" + c3 + ";opacity:1;fill-opacity:1;fill-rule:evenodd;stroke-width:3.86077285;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1");
				setAttrRecur(o, "path", "pathLeg", "style", "stroke:"+c2+";fill:none;opacity:1;fill-opacity:1;fill-rule:evenodd;stroke-width:3.86077285;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1");
				setAttrRecur(o, "path", "pathSmile", "style", "opacity:1;fill:none;fill-opacity:1;fill-rule:evenodd;stroke:"+c2+";stroke-width:1.28692424;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1");
				const float w=11.3653;
				const float end=0.3f;
				const float w1=p4*w*(1.0f-end)+end*w;
				const float w2=w-w1;
				const float ranA=M_PI*0.5f;
				const float minA1=1;
				const float a1=(p5*ranA)+minA1;
				const float x1=sin(a1)*w1;
				const float y1=cos(a1)*w1;
				const float minA2=a1*0.5f;
				const float a2=(p6*ranA)+minA2;
				const float x2=sin(a2)*w2;
				const float y2=cos(a2)*w2;
				setAttrRecur(o, "path", "pathLeg", "d", "m -2297.5041,-441.98664 "+QString::number(x1)+"," +QString::number(y1)+" "+QString::number(x2)+"," +QString::number(y2));
				dirty=false;
			}
		}
	}
}

void Identicon::setSvgURL(QString url){
	this->url=url;
	dirty=true;
}

void Identicon::setIdenticonData(quint64 data){
	if(0==data){
		//data=9693658694970463214;
		data=UniquePlatformFingerprint::getInstance().platform().getQuint64();
		//qDebug()<<"Identicon defaulting to fingerprint: "<<data<<"  ("<<UniquePlatformFingerprint::getInstance().platform().getHEX()<<")";
	}
	if(this->data!=data){
		this->data=data;
		dirty=true;
	}
}
