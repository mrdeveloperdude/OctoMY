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
			delete[] mScratchBuffer;
		}
		if(nullptr==mScratchBuffer) {
			mScratchBuffer = new QRgb[psz];
		}
		const qreal c1r=(qreal)qRed(mColor1)/255.0;
		const qreal c1g=(qreal)qGreen(mColor1)/255.0;
		const qreal c1b=(qreal)qBlue(mColor1)/255.0;
		const qreal c2r=(qreal)qRed(mColor2)/255.0;
		const qreal c2g=(qreal)qGreen(mColor2)/255.0;
		const qreal c2b=(qreal)qBlue(mColor2)/255.0;
		QImage scratchImage((uchar*)mScratchBuffer, w,h, QImage::Format_ARGB32);
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				qreal dx=(x-w2);
				qreal dy=(y-h2);
				qreal d=dx*dx+dy*dy;
				const qreal df=d/(sz2*sz2);
				if(df<0.1-blurEdge) {
					mScratchBuffer[y * h + x] = qRgba(21,17,13, 0);
					continue;
				}
				if(df>1.0) {
					mScratchBuffer[y * h + x] = qRgba(217,209,225,0);
					continue;
				}
				d=0==d?0.01:sqrt(d);
				auto a = atan2(dy, dx);
				dx/=d;
				dy/=d;
				qreal fade=1.0-cos((df-blurEdge)*M_PI*2.0*(1+blurEdge));
				qreal al=qBound(0.0,fade*10.0,1.0);
				qreal ad=mSimplexRipples.sample(1337.0+d*mRingRes*2.0,mBaseX,mBaseY)*0.1+a;
				qreal s=qMax(0.0,mSimplexStreaks.sample(mBaseX+cos(ad)*(mRingStartRadius+df),mBaseY+sin(ad)*(mRingStartRadius+df), 0)*fade);
				auto cv=df+s*mColorFollow;
				auto const cvi=1.0-cv;
				auto rr=filmic2((((c1r*cv+c2r*cvi)*s)*mContrast),mToneCutoff,mToneExposure);
				auto gg=filmic2((((c1g*cv+c2g*cvi)*s)*mContrast),mToneCutoff,mToneExposure);
				auto bb= filmic2((((c1b*cv+c2b*cvi)*s)*mContrast),mToneCutoff,mToneExposure);
				int r=qBound(0,(int)(rr*255.0),255);
				int g=qBound(0,(int)(gg*255.0),255);
				int b=qBound(0,(int)(bb*255.0),255);
				mScratchBuffer[y * h + x] = qRgba(r,g,b,(int)(al*255.0));

			}
		}
		painter.drawImage(0, 0, scratchImage);
		const int swatchSize=50;
		painter.setBrush(QColor::fromRgb(mColor1));
		painter.drawRect(QRect(0,0,swatchSize,swatchSize));
		painter.setBrush(QColor::fromRgb(mColor2));
		painter.drawRect(QRect(swatchSize,0,swatchSize,swatchSize));
		//image.save("/tmp/iris.png");
	}
}



void IrisRendrer::setParameter(quint32 id, qreal value)
{
	switch(id) {
	case(0): {
		mSimplexStreaks.setOctaves(1+(int)floor(value*20));
	}
	break;

	case(1): {
		mSimplexStreaks.setPersistence(value+0.3);
	}
	break;
	case(2): {
		mSimplexStreaks.setScale(value*0.1+0.2);
	}
	break;
	case(3): {
		mSimplexRipples.setOctaves(1+(int)floor(value*5));
	}
	break;
	case(4): {
		mSimplexRipples.setPersistence(value*0.1);
	}
	break;
	case(5): {
		mSimplexRipples.setScale(value*0.1);
	}
	break;
	case(6): {
		mRingStartRadius=1.0+value*30.0;
	}
	break;
	case(7): {
		mPupilSize=value;
	}
	break;
	case(8): {

	}
	break;
	case(9): {
		mRingRes=value*0.1;
	}
	break;
	case(10): {
		mContrast=value*3.0+0.15;
	}
	break;
	case(11): {
		mOval=value;
	}
	break;
	case(12): {
		mColorFollow=value;
	}
	break;
	case(13): {
		mBaseX=value*1000.0;
	}
	break;

	case(14): {
		mBaseY=value*1000.0;
	}
	break;

	case(15): {
		mColor1=QColor::fromHslF(value,1.0,0.5).rgb();
	}
	break;

	case(16): {
		mColor2=QColor::fromHslF(value,1.0,0.5).rgb();
	}
	break;

	case(17): {
		mToneCutoff=value*0.23+0.02;
	}
	break;

	case(18): {
		mToneExposure=1.0+value*0.6;
	}
	break;

	case(19): {

	} break;


	}
}













