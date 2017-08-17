#include "IrisRendrer.hpp"



#include <QRect>
#include <QPainter>
#include <QDebug>


IrisRendrer::IrisRendrer(Personality &p)
	: mPersonality(p)
{

}

static inline qreal filmic2(qreal v, qreal cutoff, qreal exposure)
{
	qreal value = exposure * v;
	value += (cutoff * 2.0 - value) * qBound(0.0 , cutoff * 2.0 - value, 1.0) * (0.25 / cutoff) - cutoff;
	return (value * (6.2f * value + 0.5f)) / (value * (6.2f * value + 1.7f) + 0.06f);
}


int psz=0;
// TODO: figure out an elegant way to pass around identicon colors
void IrisRendrer::draw(QRect &rect, QPainter &painter)
{
	const int w=rect.width();
	const int h=rect.height();
	const int w2=w/2;
	const int h2=h/2;
	const qreal sz=qMin(w,h);
	const qreal sz2=sz*0.5;
	const qreal blurEdge=0.05;

	{
		if(psz!=w*h) {
			psz=w*h;
			delete[] pixels;
			delete[] pixelsF;
		}
		if(nullptr==pixels) {
			pixels = new QRgb[psz];
		   // pixelsF = new qreal[3*psz];
		}

		qreal lo=1, hi=-1;

		const qreal c1r=(qreal)qRed(c1)/255.0;
		const qreal c1g=(qreal)qGreen(c1)/255.0;
		const qreal c1b=(qreal)qBlue(c1)/255.0;
		const qreal c2r=(qreal)qRed(c2)/255.0;
		const qreal c2g=(qreal)qGreen(c2)/255.0;
		const qreal c2b=(qreal)qBlue(c2)/255.0;
		QImage image((uchar*)pixels, w,h, QImage::Format_ARGB32);
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				qreal dx=(x-w2);
				qreal dy=(y-h2);
				qreal d=dx*dx+dy*dy;
				const qreal df=d/(sz2*sz2);
				if(df<0.1-blurEdge) {
					pixels[y * h + x] = qRgba(21,17,13, 0);
					continue;
				}
				if(df>1.0) {
					pixels[y * h + x] = qRgba(217,209,225,0);
					continue;
				}
				d=0==d?0.01:sqrt(d);
				auto a = atan2(dy, dx);
				dx/=d;
				dy/=d;
				qreal fade=1.0-cos((df-blurEdge)*M_PI*2.0*(1+blurEdge));
				qreal al=qBound(0.0,fade*10.0,1.0);
				qreal ad=mSimplexRipples.sample(1337.0+d*ringRes*2.0,baseX,baseY)*0.1+a;
				qreal s=qMax(0.0,mSimplexStreaks.sample(baseX+cos(ad)*(ringStartRadius+df),baseY+sin(ad)*(ringStartRadius+df), 0)*fade);
				auto cv=df+s*colorFollow;
				auto const cvi=1.0-cv;
				auto rr=filmic2((((c1r*cv+c2r*cvi)*s)*contrast),cutoff,exposure);
				auto gg=filmic2((((c1g*cv+c2g*cvi)*s)*contrast),cutoff,exposure);
				auto bb= filmic2((((c1b*cv+c2b*cvi)*s)*contrast),cutoff,exposure);
				int r=qBound(0,(int)(rr*255.0),255);
				int g=qBound(0,(int)(gg*255.0),255);
				int b=qBound(0,(int)(bb*255.0),255);
				pixels[y * h + x] = qRgba(r,g,b,(int)(al*255.0));

			}
		}
		painter.drawImage(0, 0, image);
		const int swatchSize=50;
		painter.setBrush(QColor::fromRgb(c1));
		painter.drawRect(QRect(0,0,swatchSize,swatchSize));
		painter.setBrush(QColor::fromRgb(c2));
		painter.drawRect(QRect(swatchSize,0,swatchSize,swatchSize));
		image.save("/tmp/iris.png");
		qDebug()<<"HI="<<hi<<", LO="<<lo<<", SZ="<<sz;
		//delete[] pixels;
	}
}



void IrisRendrer::setParameter(QString name, qreal value)
{
	qDebug()<<name<<"="<<value;
	if("val0"==name) {
		mSimplexStreaks.setOctaves(1+(int)floor(value*20));
	} else if("val1"==name) {
		mSimplexStreaks.setPersistence(value+0.3);
	} else if("val2"==name) {
		mSimplexStreaks.setScale(value*0.1+0.2);
	}

	if("val3"==name) {
		mSimplexRipples.setOctaves(1+(int)floor(value*5));
	} else if("val4"==name) {
		mSimplexRipples.setPersistence(value*0.1);
	} else if("val5"==name) {
		mSimplexRipples.setScale(value*0.1);
	}


	else if("val6"==name) {
		ringStartRadius=1.0+value*30.0;
	} else if("val7"==name) {
		pupilSize=value;
	} else if("val8"==name) {
		edgeSize=value;
	} else if("val9"==name) {
		ringRes=value*0.1;
	} else if("val10"==name) {
		contrast=value*3.0+0.15;
	} else if("val11"==name) {
		oval=value;
	} else if("val12"==name) {
		colorFollow=value;
	} else if("val13"==name) {
		baseX=value*1000.0;
	} else if("val14"==name) {
		baseY=value*1000.0;
	} else if("val15"==name) {
		c1=QColor::fromHslF(value,1.0,0.5).rgb();
	}    else if("val16"==name) {
		c2=QColor::fromHslF(value,1.0,0.5).rgb();
	}else if("val17"==name) {
		cutoff=value*0.23+0.02;
	}else if("val18"==name) {
		exposure=1.0+value*0.6;
	}
}









































