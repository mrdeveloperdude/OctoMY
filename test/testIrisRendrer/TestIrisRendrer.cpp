#include "TestIrisRendrer.hpp"

#include "IrisRendrerTestWidget.hpp"

#include "expression/IrisRendrer.hpp"


#include <QPainter>
#include <QImage>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QSignalSpy>
#include <QTimer>
#include <QDateTime>



void TestIrisRendrer::testUI()
{
	const QImage::Format fmt=QImage::Format_ARGB32;
	const QSize sz(512,512);
	QImage im(sz,fmt);
	QPainter p(&im);
	//p.begin(&im)
	Personality pe;
	IrisRendrer ir(pe);
	QRect r(QPoint(0,0), sz);
	ir.draw(r,p);
	IrisRendrerTestWidget pv;

	auto irp=&ir;
	auto pvp=&pv;
	auto pp=&p;
	auto rp=&r;
	auto imp=&im;
	connect(&pv, &IrisRendrerTestWidget::valueChanged, this, [=](quint32 id, qreal value) {
		irp->setParameter(id, value);
		irp->draw(*rp,*pp);
		pvp->setImage(*imp);
		pvp->update();
		//qDebug()<<name<<" changed to "<<value;
	}
		   );

	pv.setImage(im);
	pv.setWindowTitle("IrisRendrer");
	pv.show();


	QSignalSpy spy(&pv, SIGNAL(close()));

	while(0==spy.count()) {
		spy.wait(100);
	}

}

void TestIrisRendrer::testSave()
{
	qsrand(QDateTime::currentMSecsSinceEpoch());
	const QImage::Format fmt=QImage::Format_ARGB32;
	const QSize sz(512,512);
	QImage im(sz,fmt);
	QPainter p(&im);
	Personality pe;
	IrisRendrer ir(pe);
	QRect r(QPoint(0,0), sz);
	for(int i=0;i<500;++i){
		for(int p=0;p<20;++p){
			ir.setParameter(p,(qreal)(qrand()%1000)/1000.0);
		}

		ir.draw(r,p);
		im.save("/tmp/iris"+QString::number(i)+".png");
	}



}

QTEST_MAIN(TestIrisRendrer)
