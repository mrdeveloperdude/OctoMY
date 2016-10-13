#include "Identicon.hpp"
#include "basic/UniquePlatformFingerprint.hpp"
#include "random/RNG.hpp"

#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>
#include <QDebug>
#include <QFile>



Identicon::Identicon(PortableID &id)
	: dirty(true)
	, mID(id)
	, rng(RNG::sourceFactory("mt"))
{
}

Identicon::Identicon()
	: dirty(true)
	, rng(RNG::sourceFactory("mt"))
{

}

Identicon::~Identicon(){
	delete rng;
}



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


static QString hsla(qreal h, qreal s, qreal l, qreal a){
	return "hsla("+QString::number(h*360.0f)+", "+QString::number((int)(s*100))+"%, "+QString::number((int)(l*100))+"%, "+QString::number(a)+")";
}


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


float Identicon::frand(){
	return rng->generateReal1();
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
	//qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
	zoom=(zoom<0)?0:(zoom>1)?1:zoom;
	qreal izoom=1.0-zoom;
	return low*izoom+high*zoom;
}

QPixmap Identicon::pixmap(qint32 w,qint32 h,qreal zoom){
	regenerateIdenticon();
	QSvgRenderer svg( doc.toByteArray() );
	QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
	//qDebug()<<"Generating pixmap from identicon with size: "<<ds;
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
	for(int step=0;step<limbSteps;++step){
		angle+=(((top?p5:p6)-0.5)*2 *M_PI_2)/limbSteps;
		const float stepX=cos(angle)*limbStepSize*(left?-1:1);
		const float stepY=sin(angle)*limbStepSize*(top?1:-1);
		m+=QString::number(stepX)+", "+QString::number(stepY)+" ";
	}
	//qDebug()<<"LIMB: "<<name<<m << " dirAngle="<<dirAngle<<" spreadAngle=" << spreadAngle<<" startAngle=" << startAngle;
	setAttrRecur(o, "path", name, "d", m);
	setAttrRecur(o, "path", name, "style", limbStyle);

}

void Identicon::regenerateIdenticon(){
	if(dirty){
		QString url;
		switch(mID.type()){
			default:
			case(TYPE_AGENT):url=":/icons/agent.svg";break;
			case(TYPE_REMOTE):url=":/icons/remote.svg";break;
			case(TYPE_HUB):url=":/icons/hub.svg";break;
		}

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
				rng->init((mID.id()+"OctoMY™ Identicon").toUtf8());

				qreal p1=frand();
				qreal p2=frand();
				qreal p3=frand();
				qreal p4=frand();
				qreal p5=frand();
				qreal p6=frand();
				qreal p7=frand();
				qreal p8=frand();
				qreal p9=frand();
				qreal p10=frand();

				//qDebug()<<"Identicon params: "<<mID.id()<<p1<<p2<<p3<<p4<<p5<<p6<<p7;
				const QString body_color_high=hsl(p1,0.75f,0.55f);
				const QString body_color_low=hsl(p1,0.75f,0.425f);
				const QString background_color_high=hsl(p2,0.75f,0.25f);
				const QString background_color_low=hsl(p2,0.75f,0.20f);
				const QString limb_color=hsl(p3,0.75f,0.55f);
				QDomElement o=doc.documentElement();

				DiscoveryType type=mID.type();

				if(TYPE_AGENT==type){
					setAttrRecur(o, "stop", "agent_body_color_stop1", "style", "stop-color:"+body_color_high+";stop-opacity:1");
					setAttrRecur(o, "stop", "agent_body_color_stop2", "style", "stop-color:"+body_color_low+";stop-opacity:1");
					setAttrRecur(o, "stop", "agent_background_color_stop1", "style", "stop-color:"+background_color_high+";stop-opacity:1");
					setAttrRecur(o, "stop", "agent_background_color_stop2", "style", "stop-color:"+background_color_low+";stop-opacity:1");
					setAttrRecur(o, "path", "agent_face", "style", "fill:none;stroke:"+limb_color+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");

					const QString limbStyle="fill:none;stroke:"+limb_color+";stroke-width:6.12427092;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1";

					const float topx=-882.65796;
					const float topy=601.55292;
					const float botx=-882.65796;
					const float boty=622.47913;

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
					//setAttrRecur(o, "path", "agent_limb", "d", "m -858.08772,611.42291 -3.54117,0 -3.6582,-5.61549");

					erectLimb(o,limbStyle, false, false,false,p4,p5,p6,p7,p8,p9);
					erectLimb(o,limbStyle, true,false,false,p4,p5,p6,p7,p8,p9);
					erectLimb(o,limbStyle, false,true,false,p4,p5,p6,p7,p8,p9);
					erectLimb(o,limbStyle, true,true,false,p4,p5,p6,p7,p8,p9);
					erectLimb(o,limbStyle, false,false,true,p4,p5,p6,p7,p8,p9);
					erectLimb(o,limbStyle, true,false,true,p4,p5,p6,p7,p8,p9);
					erectLimb(o,limbStyle, false,true,true,p4,p5,p6,p7,p8,p9);
					erectLimb(o,limbStyle, true,true,true,p4,p5,p6,p7,p8,p9);
				}
				else if(TYPE_REMOTE==type){
					setAttrRecur(o, "stop", "remote_body_color_stop1", "style", "stop-color:"+body_color_high+";stop-opacity:1");
					setAttrRecur(o, "stop", "remote_body_color_stop2", "style", "stop-color:"+body_color_low+";stop-opacity:1");
					setAttrRecur(o, "stop", "remote_background_color_stop1", "style", "stop-color:"+background_color_high+";stop-opacity:1");
					setAttrRecur(o, "stop", "remote_background_color_stop2", "style", "stop-color:"+background_color_low+";stop-opacity:1");
					setAttrRecur(o, "path", "remote_face", "style", "fill:none;stroke:"+limb_color+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");

					setAttrRecur(o, "circle", "remote_outer_ring", "style", "fill:none;stroke:"+limb_color+";stroke-width:" + QString::number(0.25+p4*3)+ ";stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
					setAttrRecur(o, "circle", "remote_middle_ring", "style", "fill:none;stroke:"+limb_color+";stroke-width:" + QString::number(0.20+p5*3.5)+ ";stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
					setAttrRecur(o, "circle", "remote_inner_ring", "style", "fill:none;stroke:"+limb_color+";stroke-width:" + QString::number(0.25+p6*3)+ ";stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");

					setAttrRecur(o, "circle", "remote_middle_ring", "r", QString::number(11+ p7*6.0));

					setAttrRecur(o, "circle", "remote_ball", "style", "stroke:none; fill:"+limb_color+";");
				}
				else if(TYPE_HUB==type){
					setAttrRecur(o, "stop", "hub_body_color_stop1", "style", "stop-color:"+body_color_high+";stop-opacity:1");
					setAttrRecur(o, "stop", "hub_body_color_stop2", "style", "stop-color:"+body_color_low+";stop-opacity:1");
					setAttrRecur(o, "stop", "hub_background_color_stop1", "style", "stop-color:"+background_color_high+";stop-opacity:1");
					setAttrRecur(o, "stop", "hub_background_color_stop2", "style", "stop-color:"+background_color_low+";stop-opacity:1");
					setAttrRecur(o, "path", "hub_face", "style", "fill:none;stroke:"+limb_color+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
					setAttrRecur(o, "path", "hub_brain", "style", "fill:none;stroke:"+limb_color+";stroke-width:1.70119631;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1");
					setAttrRecur(o, "path", "hub_brain", "transform", "rotate("+QString::number(p4*360.0)+", -860.79895, 612.97186)");
				}
				/*
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
					*/

				dirty=false;
			}
		}
	}
}

/*
void Identicon::setSvgURL(QString url){
	this->url=url;
	dirty=true;
}*/

void Identicon::setPortableID(PortableID id){
	mID=id;
	dirty=true;
	regenerateIdenticon();
}
