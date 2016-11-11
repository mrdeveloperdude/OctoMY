#include "StressQR.hpp"

#include "../libqr/QRPainter.hpp"
#include "../libzbar/ZBarScanner.hpp"
#include "../libutil/utility/widgets/PixViewer.hpp"
#include "../libutil/utility/ScopedTimer.hpp"

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

/*!
 * \brief StressQR::stress will generate random data encoded into
 * randomly sized QR images, transformed randomly into a scene image
 * then this scene image is probed with zbar to see if the original data can be
 * retreived.
 *
 * This is repeated continuously for some time as a stress test while
 * an error rate is calculated.
 */


static QString randomString(int sz)
{
//	return "gidtphcgnxklqczpcmmyzqgcwobwwfffiovhhvpqchihaftxqe";

	//static const QString alphabet="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!\"#¤%&/()= _-.,:;^æøåÆÅØ";
	static const QString alphabet="abcdefghijklmnopqrstuvwxyz";
	static const int alphalen=alphabet.size();
	QString out;
	for(int i=0; i<sz; ++i) {
		out.append(alphabet[ qrand()%alphalen ]);
	}
	return out;

}


static qreal frand()
{
	return ((qreal)qrand())/((qreal)RAND_MAX);
}



void StressQR::stress()
{
	qDebug()<<"Entered stress()";
	//QPixmap loaded("/home/lennart/Downloads/qrcode.png");
	PixViewer pv;
	pv.show();
	ZBarScanner scanner;
	QWidget *wg=new QWidget(nullptr);
	QGridLayout *gridLayout = new QGridLayout(wg);
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	wg->setSizePolicy(sizePolicy);
	QLabel *label=new QLabel(wg);
	gridLayout->addWidget(label,0,0,1,1);
	wg->show();
	qApp->processEvents();
	quint64 lastTime=QDateTime::currentMSecsSinceEpoch();
	int ct=0;
	int hits=0;
	const qreal dw=3;
	QVector<qreal> dashes;
	dashes << 1 << 2;
	QPen pen(Qt::red, dw, Qt::CustomDashLine, Qt::RoundCap, Qt::RoundJoin);

	const int globalBaseSize=150;
	const int globalSize=300;
	const int globalSafetyMargin=(globalSize+globalBaseSize)*0.5;
	for(int i=0; i<10000; ++i) {
		const quint64 now=QDateTime::currentMSecsSinceEpoch();
		const quint64 interval=now-lastTime;
		if(interval>1000) {
			qDebug()<<"Running QR test "<<i<<" @ "<<ct<<"/sec ( "<<hits<<"hits "<<(((hits+1)*100)/(ct+1))<<"%)";
			lastTime=now;
			ct=0;
			hits=0;
		}
		ct++;

		// Generate QR image
		const int qrwidth=globalBaseSize+qrand()%globalSize;
		const int strl=0x20+qrand()%100;
		const QString data=randomString(strl);
		const QSize qrsize(qrwidth,qrwidth);
		QColor fg =QColor::fromHslF(frand(),frand(),frand()*0.75);//Relatively dark color
		QColor bg =QColor::fromHslF(frand(),frand(),frand()*0.20+0.80);//Relatively light color
		QPixmap qrpx(qrwidth,qrwidth);
		qrpx.fill(bg);
		QPainter qrp(&qrpx);
		paintQR(qrp,qrsize,data,fg);
		pv.setPixmap(qrpx);

		// Generate scene image
		const int w=((qrwidth+qrand()%globalSize+globalSafetyMargin)/4 )*4;//zbar only supports images of width divisable by 4
		const int h=qrwidth+qrand()%globalSize+globalSafetyMargin;
		const qreal range=360.0;
		const qreal angle1=frand()*range-range*0.5;
		const qreal angle2=frand()*M_PI*2.0;
		const qreal distance=frand()*(qMin(w,h)*0.7-qrwidth)/2;
		QPixmap px(w,h);
		QColor bg2 =QColor::fromHslF(frand(),frand(),frand()*0.75);//Relatively dark color
		px.fill(bg2);
		QPainter p(&px);
		const int cx=cos(angle2)*distance+w/2;
		const int cy=sin(angle2)*distance+h/2;
		p.save();
		p.translate(cx,cy);
		p.rotate(angle1);
		p.drawPixmap(-qrwidth/2,-qrwidth/2,qrpx);
		p.restore();
		QList<ZScanResult> found;
		{
			//ScopedTimer st("zbar scan time");
			found=scanner.scan(px);
		}
		if(found.size()>0) {
			hits++;
			qDebug()<<"Input: "<<data;

			// Draw outlines found by zbar with dotted red poly-lines
			pen.setDashPattern(dashes);
			p.setPen(pen);
			p.setBrush(Qt::NoBrush);
			p.setRenderHint(QPainter::Antialiasing);
			const int r=15;
			for(ZScanResult res:found) {
				p.drawPath(res.outline);
				QPainterPath::Element firstPoint=res.outline.elementAt(0);
				p.drawEllipse(firstPoint.x-r,firstPoint.y-r,r*2,r*2);
			}
			//Show on widget
			label->setPixmap(px);
			qApp->processEvents();
			//QTest::qSleep(1000);
		}
	}
	wg->deleteLater();
	label->deleteLater();
	gridLayout->deleteLater();
	qApp->processEvents();
}


QTEST_MAIN(StressQR)
