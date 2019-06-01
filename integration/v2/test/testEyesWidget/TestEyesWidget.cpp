#include "TestEyesWidget.hpp"

#include "expression/EyesWidget.hpp"
#include "expression/EyeRendrer.hpp"

#include "security/PortableID.hpp"
#include "utility/graphics/widgets/PixViewer.hpp"
#include "utility/random/Random.hpp"
#include "utility/graphics/Graphics.hpp"

#include "Utility_test.hpp"

#include "uptime/ConnectionType.hpp"

#include <QSignalSpy>
#include <QVector2D>
#include <QPainter>


void TestEyesWidget::testRendrer()
{
	const quint32 sz=1024;
	QVector2D center(0.5,0.5);
	QVector2D nil;
	qreal slant(0.1*M_PI);
	QColor irisColor("#2d8ac9");
	EyeRendrer er(center, slant, irisColor), *erp=&er;;

	er.update();
	QImage im(sz,sz,QImage::Format_ARGB32), *imp=&im;

	PixViewer pix(nullptr, "Test EyeRendrer"), *pixp=&pix;

	pix.show();

	QTimer t;
	t.start(10);
	connect(&t,&QTimer::timeout, this, [=]() {
		if(utility::random::frand()>0.1)erp->setExpression(QVector2D(utility::random::frand(),utility::random::frand())
														   , QVector2D(utility::random::frand(), utility::random::frand())
														   , QVector2D(utility::random::frand(), utility::random::frand()));
		if(utility::random::frand()>0.1)erp->setSteer(QVector2D(utility::random::frand(),utility::random::frand()));
		if(utility::random::frand()>0.1)erp->setBlink(utility::random::frand());
		if(utility::random::frand()>0.1)erp->setColor(utility::graphics::randomColor());
		//if(utility::frand()>0.1)er.setIrisImage(QSharedPointer<QImage>);
		if(utility::random::frand()>0.1)erp->update();
		imp->fill(Qt::blue);
		{
			QPainter p(imp);
			p.drawLine(QPoint(0,0), QPoint(sz,sz));
			p.setPen(Qt::NoPen);
			p.setRenderHint(QPainter::Antialiasing,true);
			const qreal s=utility::random::frand()*sz;// Width is most important.
			p.translate(s/2,s/2);

			p.scale(s,s);
			erp->paint(p);
		}
		pixp->setImage(*imp);

	}, OC_CONTYPE);

	test::utility::waitForUIEnd(&pix);
}


void TestEyesWidget::testWidget()
{
	PortableID pid, *pidp=&pid;
	EyesWidget ew, *ewp=&ew;
	ew.show();
	QTimer t;
	t.start(10);
	connect(&t,&QTimer::timeout, this, [=]() {
		pidp->setID(utility::random::randomByteArray(10).toBase64());
		ewp->setPortableID(*pidp);
	}, OC_CONTYPE);

	test::utility::waitForUIEnd(&pix);

}


OC_TEST_MAIN(test, TestEyesWidget)
