#include "TestHaar.hpp"

#include "haar/IntegralImage.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"


static inline void debugOut(const QImage &grayscale){
	qDebug() << "Grayscale values:";
	for (int y = 0; y < grayscale.height(); ++y) {
		for (int x = 0; x < grayscale.width(); ++x) {
			qDebug() << bt601_gray(grayscale.pixel(x, y));
		}
	}		
}

static inline void debugOut(const IntegralImage<int> &integralImage){
	qDebug() << "Integral Image values:";
	for (int y = 0; y < integralImage.height(); ++y) {
		for (int x = 0; x < integralImage.width(); ++x) {
			qDebug() << integralImage.sumInRect(QRect(0, 0, x + 1, y + 1));
		}
	}
}

void TestHaar::testIntegralImage()
{
	{
		IntegralImage<int> integralImage;
		QVERIFY(integralImage.size().isEmpty());
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 10, 10)), 0);
	}
	{
		QImage grayscale(3, 3, QImage::Format_Grayscale8);
		grayscale.setPixel(0, 0, qRgb(10, 10, 10));  // bt601_gray: 10
		grayscale.setPixel(1, 0, qRgb(20, 20, 20));  // bt601_gray: 20
		grayscale.setPixel(2, 0, qRgb(30, 30, 30));  // bt601_gray: 30
		grayscale.setPixel(0, 1, qRgb(40, 40, 40));  // bt601_gray: 40
		grayscale.setPixel(1, 1, qRgb(50, 50, 50));  // bt601_gray: 50
		grayscale.setPixel(2, 1, qRgb(60, 60, 60));  // bt601_gray: 60
		grayscale.setPixel(0, 2, qRgb(70, 70, 70));  // bt601_gray: 70
		grayscale.setPixel(1, 2, qRgb(80, 80, 80));  // bt601_gray: 80
		grayscale.setPixel(2, 2, qRgb(90, 90, 90));  // bt601_gray: 90
		IntegralImage<int> integralImage(grayscale);
		debugOut(grayscale);
		debugOut(integralImage);
		QCOMPARE(integralImage.size(), grayscale.size());
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 1, 1)), 10);   // (10) -> 10
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 2, 2)), 120);  // 10 + 20 + 40 + 50 = 120
		QCOMPARE(integralImage.sumInRect(QRect(1, 1, 1, 1)), 50);   // (50) -> 50
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 3, 3)), 450);  // Sum of all values
		QCOMPARE(integralImage.sumInRect(QRect(-5, -5, 10, 10)), 450); // Full sum, clipped to bounds
	}
	{
		QImage whiteImage(5, 5, QImage::Format_Grayscale8);
		whiteImage.fill(Qt::white);  // bt601_gray(Qt::white) = 255
		IntegralImage<int> integralImage(whiteImage);
		debugOut(whiteImage);
		debugOut(integralImage);
		QCOMPARE(integralImage.size(), whiteImage.size());
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 5, 5)), 255 * 5 * 5);
		QCOMPARE(integralImage.sumInRect(QRect(2, 2, 2, 2)), 255 * 4);
	}
	{
		QImage firstImage(3, 3, QImage::Format_Grayscale8);
		firstImage.fill(Qt::black);  // bt601_gray(Qt::black) = 0
		QImage secondImage(5, 5, QImage::Format_Grayscale8);
		secondImage.fill(Qt::white); // bt601_gray(Qt::white) = 255
		IntegralImage<int> integralImage(firstImage);
		debugOut(firstImage);
		debugOut(integralImage);
		QCOMPARE(integralImage.size(), firstImage.size());
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 3, 3)), 0);
		integralImage.set(secondImage);
		debugOut(secondImage);
		debugOut(integralImage);
		QCOMPARE(integralImage.size(), secondImage.size());
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 5, 5)), 255 * 5 * 5);
	}
	{
		QImage grayscale(4, 4, QImage::Format_Grayscale8);
		grayscale.fill(Qt::white);  // bt601_gray(Qt::white) = 255
		IntegralImage<int> integralImage(grayscale);
		debugOut(grayscale);
		debugOut(integralImage);
		QCOMPARE(integralImage.sumInRect(QRect(2, 2, 10, 10)), 255 * 2 * 2);
		QCOMPARE(integralImage.sumInRect(QRect(5, 5, 2, 2)), 0);
	}
	{
		QImage singlePixel(1, 1, QImage::Format_Grayscale8);
		singlePixel.setPixel(0, 0, qRgb(42, 42, 42));  // bt601_gray: 42
		IntegralImage<int> integralImage(singlePixel);
		debugOut(singlePixel);
		debugOut(integralImage);
		QCOMPARE(integralImage.size(), QSize(1, 1));
		QCOMPARE(integralImage.sumInRect(QRect(0, 0, 1, 1)), 42);  // bt601_gray(42,42,42) -> 42
		QCOMPARE(integralImage.sumInRect(QRect(-1, -1, 2, 2)), 42); // Full sum, clipped to bounds
	}
}

