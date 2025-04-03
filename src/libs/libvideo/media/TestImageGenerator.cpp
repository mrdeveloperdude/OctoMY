#include "TestImageGenerator.hpp"
#include <QPainter>
#include <QColor>
#include <QRect>
#include <QPoint>

QRect TestImageGenerator::calculateTargetRect(const QRect &rect, double aspect)
{
	int targetWidth = rect.width();
	int targetHeight = rect.height();
	double rectAspect = double(targetWidth) / double(targetHeight);
	if (rectAspect > aspect) {
		targetWidth = int(targetHeight * aspect);
	} else {
		targetHeight = int(targetWidth / aspect);
	}
	int x = rect.x() + (rect.width() - targetWidth) / 2;
	int y = rect.y() + (rect.height() - targetHeight) / 2;
	return QRect(x, y, targetWidth, targetHeight);
}

QPixmap TestImageGenerator::generateTestFrame(const QSize &size)
{
	// Create a pixmap of the specified size and fill it with black.
	QPixmap pixmap(size);
	pixmap.fill(Qt::black);
	
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	// Define the aspect ratio for the SMPTE pattern (4:3).
	const double patternAspect = 4.0 / 3.0;
	QRect fullRect(QPoint(0, 0), size);
	QRect targetRect = calculateTargetRect(fullRect, patternAspect);
	
	// Define the 7 vertical SMPTE color bars.
	const int numBars = 7;
	int barWidth = targetRect.width() / numBars;
	QColor colors[numBars] = {
		Qt::white,
		Qt::yellow,
		Qt::cyan,
		Qt::green,
		Qt::magenta,
		Qt::red,
		Qt::blue
	};
	
	// Draw each bar within the target rectangle.
	for (int i = 0; i < numBars; i++) {
		QRect barRect(targetRect.left() + i * barWidth,
					  targetRect.top(),
					  barWidth,
					  targetRect.height());
		painter.fillRect(barRect, colors[i]);
	}
	
	// Optionally, draw a border around the SMPTE area.
	painter.setPen(Qt::black);
	painter.drawRect(targetRect);
	
	return pixmap;
}

QPixmap TestImageGenerator::generatePM5544TestImage(const QSize &size)
{
	// Create a pixmap of the specified size and fill it with black.
	QPixmap pixmap(size);
	pixmap.fill(Qt::black);
	
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	int width = size.width();
	int height = size.height();
	
	// Divide the image horizontally:
	// Top 60% for color bars, bottom 40% for a grayscale ramp.
	int topHeight = int(height * 0.6);
	int bottomHeight = height - topHeight;
	
	QRect topRect(0, 0, width, topHeight);
	QRect bottomRect(0, topHeight, width, bottomHeight);
	
	// --- Top Section: 7 Vertical Color Bars ---
	const int numTopBars = 7;
	int topBarWidth = width / numTopBars;
	// Define the colors for the top section.
	// (These colors are chosen to be similar to a typical PM5544 pattern.)
	QColor topColors[numTopBars] = { 
		Qt::white,
		Qt::yellow,
		Qt::cyan,
		Qt::green,
		Qt::magenta,
		Qt::red,
		Qt::blue
	};
	for (int i = 0; i < numTopBars; i++) {
		QRect barRect(i * topBarWidth, 0, topBarWidth, topHeight);
		painter.fillRect(barRect, topColors[i]);
	}
	
	// --- Bottom Section: 8 Grayscale Steps ---
	const int numBottomSteps = 8;
	int stepWidth = width / numBottomSteps;
	for (int i = 0; i < numBottomSteps; i++) {
		// Calculate gray level from 0 (black) to 255 (white)
		int grayValue = int(255.0 * i / (numBottomSteps - 1));
		QColor gray(grayValue, grayValue, grayValue);
		QRect stepRect(i * stepWidth, topHeight, stepWidth, bottomHeight);
		painter.fillRect(stepRect, gray);
	}
	
	// Optionally, draw borders around the sections.
	painter.setPen(Qt::black);
	painter.drawRect(topRect);
	painter.drawRect(bottomRect);
	
	return pixmap;
}
