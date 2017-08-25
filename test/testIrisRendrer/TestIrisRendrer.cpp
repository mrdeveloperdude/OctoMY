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
	IrisRendrer ir;
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
	IrisRendrer ir;
	QRect r(QPoint(0,0), sz);
	for(int i=0; i<500; ++i) {
		for(int p=0; p<20; ++p) {
			ir.setParameter(p,(qreal)(qrand()%1000)/1000.0);
		}

		ir.draw(r,p);
		im.save("/tmp/iris"+QString::number(i)+".png");
	}



}



void TestIrisRendrer::testHuge()
{
	qsrand(QDateTime::currentMSecsSinceEpoch());
	const QImage::Format fmt=QImage::Format_ARGB32;
	int s=2160;
	const QSize sz(s,s);
	QImage im(sz,fmt);
	QPainter p(&im);
	IrisRendrer ir;
	QRect r(QPoint(0,0), sz);
	PortableID pid;
	pid.setID("44729B7E0E1ACF15A8E121E9A6A38B328E900B40663163258E44BA26C5A748D9");
	ir.setPortableID(pid);
	ir.draw(r,p);
	im.save("/tmp/iris_huge.png");

}



QTEST_MAIN(TestIrisRendrer)
