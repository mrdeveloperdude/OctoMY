#ifndef TESTIMAGEGENERATOR_HPP
#define TESTIMAGEGENERATOR_HPP

#include <QPixmap>
#include <QSize>
#include <QRect>

class TestImageGenerator {
public:
	// Returns a QPixmap containing a SMPTE color bars test frame.
	// The test frame is drawn in the largest centered rectangle with a 4:3 aspect ratio,
	// and the remainder of the pixmap is filled with black.
	static QPixmap generateTestFrame(const QSize &size);
	
	// Returns a QPixmap containing a PM5544 test image.
	// This simplified PM5544 image divides the area horizontally:
	// - Top 60%: seven vertical color bars.
	// - Bottom 40%: eight grayscale steps.
	static QPixmap generatePM5544TestImage(const QSize &size);
	
private:
	// Helper: Compute a centered rectangle inside 'rect' that preserves the given aspect ratio.
	static QRect calculateTargetRect(const QRect &rect, double aspect);
};

#endif // TESTIMAGEGENERATOR_HPP
