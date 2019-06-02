#include "IrisRendrer.hpp"

#include "random/RNG.hpp"


#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QRect>
#include <QPainter>
#include <QDebug>



IrisRendrer::IrisRendrer()
	: mOval(0.0)
	, mPupilSize(1.0)
	, mRingRes(1.0)
	, mRingStartRadius(1.0)
	, mContrast(1.0)
	, mColorFollow(1.0)
	, mBaseX(0.0)
	, mBaseY(0.0)
	, mToneExposure(1.0)
	, mToneCutoff(0.025)
	, mColor1(QColor("#8294aa").rgb())
	, mColor2(QColor("#752e0b").rgb())
	, mScratchBuffer(nullptr)
	, psz(0)
	, mDebugMode(false)
{
	OC_METHODGATE();
}


PortableID IrisRendrer::portableID()
{
	OC_METHODGATE();
	return mPid;
}
void IrisRendrer::setPortableID(PortableID pid)
{
	OC_METHODGATE();
	mPid=pid;
	mPersonality.setID(pid.id(), Personality::IRIS);
	mPersonality.reset();
	mColors.setID(pid.id());
	for(quint32 i=0; i<20; ++i) {
		setParameter(i, mPersonality.rng().generateReal1());
	}
	mColor1=mColors.backgroundColorLow().rgb();
	mColor2=mColors.bodyColorLow().rgb();
}


static inline qreal filmic2(qreal v, qreal cutoff, qreal exposure)
{
	OC_FUNCTIONGATE();
	qreal value = exposure * v;
	value += (cutoff * 2.0 - value) * qBound(0.0, cutoff * 2.0 - value, 1.0) * (0.25 / cutoff) - cutoff;
	return (value * (6.2 * value + 0.5)) / (value * (6.2 * value + 1.7) + 0.06);
}





// TODO: figure out an elegant way to pass around identicon colors
void IrisRendrer::draw(QRect &rect, QPainter &painter, quint32 eyeIndex)
{
	OC_METHODGATE();
	const int w=rect.width();
	const int h=rect.height();
	const int w2=w/2;
	const int h2=h/2;
	const qreal sz=qMin(w,h);
	const qreal sz2=sz*0.5;
	const qreal blurEdge=0.05;

	{
		QPainter::CompositionMode cm=painter.compositionMode();
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect(QRect(0,0,w,h), Qt::transparent);
		painter.setCompositionMode(cm);
	}
	{
		const int npsz=w*h;
		if(psz!=npsz) {
			psz=npsz;
			delete[] mScratchBuffer;
			mScratchBuffer=nullptr;
		}
		if(nullptr==mScratchBuffer) {
			mScratchBuffer = OC_NEW QRgb[static_cast<unsigned long>(psz)];
		}//
		const qreal c1r=static_cast<qreal>(qRed(mColor1)/255.0);
		const qreal c1g=static_cast<qreal>(qGreen(mColor1)/255.0);
		const qreal c1b=static_cast<qreal>(qBlue(mColor1)/255.0);
		const qreal c2r=static_cast<qreal>(qRed(mColor2)/255.0);
		const qreal c2g=static_cast<qreal>(qGreen(mColor2)/255.0);
		const qreal c2b=static_cast<qreal>(qBlue(mColor2)/255.0);
		const qreal indexX=eyeIndex*1337.0+mBaseX;
		const qreal indexY=eyeIndex*1337.0+mBaseY;
		QImage scratchImage(reinterpret_cast<uchar*>(mScratchBuffer), w,h, QImage::Format_ARGB32);
		const unsigned int inColor=qRgba(21,17,13, 0);
		const unsigned int outColor=qRgba(217,209,225,0);
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				qreal dx=(x-w2);
				qreal dy=(y-h2);
				qreal d=dx*dx+dy*dy;
				const qreal df=d/(sz2*sz2);
				if(df<0.1-blurEdge) {
					mScratchBuffer[y * h + x] = inColor;
					continue;
				}
				if(df>1.0) {
					mScratchBuffer[y * h + x] = outColor;
					continue;
				}
				d=qFuzzyCompare(0, d)?0.01: std::sqrt(d);
				const qreal a = atan2(dy, dx);
				dx/=d;
				dy/=d;
				const qreal fade=1.0-cos((df-blurEdge)*M_PI*2.0*(1+blurEdge));
				const qreal al=qBound(0.0,fade*10.0,1.0);
				const qreal ad=mSimplexRipples.sample(1337.0+d*mRingRes*2.0,indexX,indexY)*0.1+a;
				const qreal s=qMax(0.0,mSimplexStreaks.sample(indexX+cos(ad)*(mRingStartRadius+df),indexY+sin(ad)*(mRingStartRadius+df), 0)*fade);
				const qreal cv=df+s*mColorFollow;
				const qreal cvi=1.0-cv;
				const qreal rr=filmic2((((c1r*cv+c2r*cvi)*s)*mContrast),mToneCutoff,mToneExposure);
				const qreal gg=filmic2((((c1g*cv+c2g*cvi)*s)*mContrast),mToneCutoff,mToneExposure);
				const qreal bb= filmic2((((c1b*cv+c2b*cvi)*s)*mContrast),mToneCutoff,mToneExposure);
				const int r=qBound(0, static_cast<int>(rr*255.0), 255);
				const int g=qBound(0, static_cast<int>(gg*255.0), 255);
				const int b=qBound(0, static_cast<int>(bb*255.0), 255);
				mScratchBuffer[y * h + x] = qRgba(r, g, b, static_cast<int>(al*255.0));
			}
		}
		painter.drawImage(0, 0, scratchImage);
		if(mDebugMode) {
			const int swatchSize=50;
			painter.setBrush(QColor::fromRgb(mColor1));
			painter.drawRect(QRect(0,0,swatchSize,swatchSize));
			painter.setBrush(QColor::fromRgb(mColor2));
			painter.drawRect(QRect(swatchSize,0,swatchSize,swatchSize));
		}
		//image.save("/tmp/iris.png");
	}
}

void IrisRendrer::setParameter(quint32 id, qreal value)
{
	OC_METHODGATE();
	switch(id) {
	case(0): {
		mSimplexStreaks.setOctaves(1+static_cast<int>(std::floor(value*20)));
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
		mSimplexRipples.setOctaves(1+static_cast<int>(std::floor(value*5)));
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



